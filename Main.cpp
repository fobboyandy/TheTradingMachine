/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */


#include "StdAfx.h"

#ifdef _WIN32
# include <Windows.h>
# define sleep( seconds) Sleep( seconds * 1000);
#else
# include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>         // std::chrono::system_clock
#include <ctime>          // std::time_t, std::tm, std::localtime, std::mktime
#include <atomic>
#include "Broker.h"
#include "TheTradingMachine.h"
//#include "PythonSocketIPC.h"
#include "IBInterface.h"


#define FILE_DIRECTORY "D:\\Users\\fobboyandy\\Desktop\\TheTradingMachine\\TheTradingMachine\\scripts\\"

enum Dir
{
	UP,
	DOWN,
	UNDEFINED
};

struct Signal
{
	string signalType;
	size_t index;
};

const unsigned SLEEP_TIME = 10;

using namespace std;


void printCandlebar(const Candlebar& candle, ostream& outstream, string end)
{
	outstream << candle.open << "," << candle.high << "," << candle.low << "," << candle.close << "," << candle.volume << end;
}

bool done = false;
void autoTrade(const Stock* stockData, vector<string>* signalsOut)
{
	Dir prevDir = Dir::UNDEFINED;
	Candlebar prevCandle, currCandle;

	//block until at least 1 candle exists
	size_t currentStockDataSize = stockData->getCandlebarCount();
	while (!currentStockDataSize)
		currentStockDataSize = stockData->getCandlebarCount();


	size_t candleIndex = currentStockDataSize - 1;
	//we only care about the newest candles, no need to start from candle 0
	currCandle = stockData->getCandle(candleIndex);
	prevCandle = currCandle;
	vector<string> signals;

	currentStockDataSize = stockData->getCandlebarCount();

	printCandlebar(currCandle, cout, "\n");
	while (!done && candleIndex < currentStockDataSize)
	{
		//wait until a new candle comes in or process the next candle if we fell behind (only possible if we poll slower than the data input rate)
		while (candleIndex + 1 >= currentStockDataSize)
		{
			if (done && candleIndex + 1 == currentStockDataSize)
				goto End;
			currentStockDataSize = stockData->getCandlebarCount();
		}

		candleIndex++;

		//get next candle
		currCandle = stockData->getCandle(candleIndex);

		printCandlebar(currCandle, cout, " ");

		string signal = "none";

		//price increase
		if (currCandle.close > prevCandle.close)
		{
			if (prevDir == DOWN)
			{
				signal = "support";
			}
			prevDir = UP;
			cout << "UP ";
			//dirFront = Dir::UP;
		}
		//price decrease
		else if (currCandle.close < prevCandle.close)
		{
			if (prevDir == UP)
			{
				signal = "resistance";
			}
			prevDir = DOWN;
			cout << "DOWN ";
		}

		prevCandle = currCandle;
		signals.push_back(signal);
		cout << signal << endl;
	}

End:
	*signalsOut = signals;
	cout << "thread end" << endl;
}

void saveStockToFile(const vector<const Stock*>* stocks)
{
	for (size_t i = 0; i < stocks->size(); i++)
	{
		Candlebar candle = (*stocks)[0]->getCandle(0);
		string filenameTime;
		for (auto& i : candle.time)
		{
			if (i >= '0' && i <= '9')
				filenameTime.push_back(i);
		}
		string filename = FILE_DIRECTORY + (*stocks)[i]->getTicker() + filenameTime + ".csv";
		cout << filename << endl;
		fstream stockSaveFile(filename, fstream::trunc | fstream::out);
		for (size_t j = 0; j < (*stocks)[i]->getCandlebarCount(); j++)
		{
			candle = (*stocks)[i]->getCandle(j);
			stockSaveFile << candle.open << "," << candle.high << "," << candle.low << "," << candle.close << "," << candle.volume << endl;
		}
		cout << "finished writing to file" << endl;
	}
}


void requestHistoricalDataFromFile(Stock& stockData)
{

	fstream inputdata("D:\\Users\\fobboyandy\\Desktop\\TheTradingMachine\\TheTradingMachine\\scripts\\MUHistory.csv", ios::in);
	string inputline;

	Candlebar currentCandle;

	while (getline(inputdata, inputline))
	{
		stringstream s(inputline);
		string token;
		getline(s, token, ',');
		currentCandle.open = stod(token);
		getline(s, token, ',');
		currentCandle.high = stod(token);
		getline(s, token, ',');
		currentCandle.low = stod(token);
		getline(s, token, ',');
		currentCandle.close = stod(token);
		getline(s, token, ',');
		currentCandle.volume = stoi(token);
		stockData.addCandlebar(currentCandle);
		Sleep(10);
	}
}

/* IMPORTANT: always use your paper trading account. The code below will submit orders as part of the demonstration. */
/* IB will not be responsible for accidental executions on your live account. */
/* Any stock or option symbols displayed are for illustrative purposes only and are not intended to portray a recommendation. */
/* Before contacting our API support team please refer to the available documentation. */
//int main(int argc, char** argv)
//{
//	const char* host = argc > 1 ? argv[1] : "";
//	unsigned int port = argc > 2 ? atoi(argv[2]) : 0;
//	if (port <= 0)
//		port = 7496;
//	const char* connectOptions = argc > 3 ? argv[3] : "";
//	int clientId = 0;
//
//	unsigned attempt = 0;
//	bool threadStarted = false;
//
//	while(1)
//	{
//		IBInterface client;
//		//if attempt to reconnect fails after certain amount of attempts, program exits
//		if (!client.Initialize())
//		{
//			cout << "Unable to connect to neither TWS nor IB Gateway!" << endl;
//			Sleep(10000);
//			return -1;
//		}
//		string stockname;
//		cin >> stockname;
//
//		std::transform(stockname.begin(), stockname.end(), stockname.begin(), toupper);
//
//		const Stock& stockRequested = client.requestHistoricalData(stockname, "1 D", "1 min");
//
//		vector<const Stock*> stocks = {&stockRequested};
//
//		//main thread processes messages
//		while (client.isConnected())
//		{
//
//			client.processMessages();
//			if (stockRequested.isHistoricalDataCompleted())
//			{
//				//reset
//				done = false;
//				//save results to file in same thread
//				saveStockToFile(&stocks);
//				client.disconnect();
//				return 0;
//			}
//		}
//
//		//if the program was able to connect previously and suddenly fails, we restart the loop and try to reconnect
//		//if the reconnection fails, the program exits
//		cout << "Client got disconnected from either TWS or IB Gateway. Reconnecting..." << endl;
//
//	}
//	return 0;
//}




int main()
{
	Stock stockData;
	string inFilePath, outFileName;
	vector<string> signals;

	cout << "Input File Path: "; cin >> inFilePath;
	cout << "Output File Name: "; cin >> outFileName;

	thread autoTradeThread(autoTrade, &stockData, &signals);


	fstream inputdata(inFilePath, ios::in);
	string inputline;

	Candlebar currentCandle;
	while (getline(inputdata, inputline))
	{
		stringstream s(inputline);
		string token;
		getline(s, token, ',');
		currentCandle.open = stod(token);
		getline(s, token, ',');
		currentCandle.high = stod(token);
		getline(s, token, ',');
		currentCandle.low = stod(token);
		getline(s, token, ',');
		currentCandle.close = stod(token);
		getline(s, token, ',');
		currentCandle.volume = stoi(token);
		stockData.addCandlebar(currentCandle);
		//printCandlebar(currentCandle, std::cout);
		Sleep(10);
	}

	done = true;

	autoTradeThread.join();

	fstream stockSaveFile( "D:\\Users\\fobboyandy\\Desktop\\TheTradingMachine\\TheTradingMachine\\scripts\\" + outFileName + "SNR.csv", fstream::trunc | fstream::out);
	auto candlebars = stockData.getCandlebars();
	Candlebar candle;
	for (size_t i = 0; i < stockData.getCandlebarCount() && i < signals.size(); i++)
	{
		candle = candlebars[i];
		printCandlebar(candle, stockSaveFile, ",");
		stockSaveFile << signals[i] << endl;
	}
	
	stockSaveFile.close();
	inputdata.close();
	system("Pause");

	return 0;

}