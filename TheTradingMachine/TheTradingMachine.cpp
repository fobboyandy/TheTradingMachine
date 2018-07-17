#include <iostream>
#include <functional>
#include <sstream>
#include "TheTradingMachine.h"

#define TICK_CSV_ROW_SZ 12



using namespace std;

TheTradingMachine::IBInterfaceClient* TheTradingMachine::ibapi = nullptr;

TheTradingMachine::TheTradingMachine(string input):realtime(true)
{
	//
	// Check if it's a historical data input
	//
	if (input.find(".tickdat") != string::npos)
	{
		tickDataFile.open(input, ios::in);
		realtime = false;
		return;
	}
	else if (ibapi == nullptr)
	{
		ibapi = new IBInterfaceClient;
	}
	//
	// If input wasn't a tick historical file, then it is a ticker
	//
	ticker = input;
}

TheTradingMachine::~TheTradingMachine()
{
	if (realtime && tickDataFile.is_open())
	{
		tickDataFile.close();
	}
}

void TheTradingMachine::requestTicks(function<void(const Tick& tick)> callback) 
{
	//
	// If running realtime, then we submit the request to ibapi
	// Otherwise, we load the ticks from the file. 
	//
	if (realtime)
	{
		ibapi->requestRealTimeTicks(ticker, callback);
	}
	else
	{
		enum CsvIndex {
			tickTypeIndex = 0,
			timeIndex,
			priceIndex = 3,
			sizeIndex,
			canAutoExecuteIndex,
			pastLimitIndex,
			preOpenIndex,
			unreportedIndex,
			bidPastLowIndex,
			askPastHIndexigh,
			exchangeIndex
		};

		string currLine;

		//
		// For each row in the csv, parse out the values in string
		// and reconstruct the tick data.
		//
		vector<string> csvRow(TICK_CSV_ROW_SZ);
		while (getline(tickDataFile, currLine))
		{
			stringstream s(currLine);
			string token;
			Tick callbackTick;
			//
			// Get each token separated by , and reconstruct the tick
			// with each csv row
			//
			for (size_t i = 0; getline(s, token, ','); i++)
			{
				switch (i)
				{
				case tickTypeIndex:
					callbackTick.tickType = stoi(token);
					break;
				case timeIndex:
					callbackTick.time = static_cast<time_t>(stoll(token));
					break;
				case priceIndex:
					callbackTick.price = stod(token);
					break;
				case sizeIndex:
					callbackTick.size = stoi(token);
					break;
				case canAutoExecuteIndex:
					callbackTick.attributes.canAutoExecute = static_cast<bool>(stoi(token));
					break;
				case pastLimitIndex:
					callbackTick.attributes.pastLimit = static_cast<bool>(stoi(token));
					break;
				case preOpenIndex:
					callbackTick.attributes.preOpen = static_cast<bool>(stoi(token));
					break;
				case unreportedIndex:
					callbackTick.attributes.unreported = static_cast<bool>(stoi(token));
					break;
				case bidPastLowIndex:
					callbackTick.attributes.bidPastLow = static_cast<bool>(stoi(token));
					break;
				case askPastHIndexigh:
					callbackTick.attributes.askPastHigh = static_cast<bool>(stoi(token));
					break;
				case exchangeIndex:
					callbackTick.exchange = token;
					break;
				}
			}
			callback(callbackTick);	
		}
	}
}

TheTradingMachine::IBInterfaceClient::IBInterfaceClient() :
	messageProcess_(nullptr)
{
	cout << "Initializing IB Client" << endl;
	clientReady.store(false);
	clientValid.store(true);
	threadRunning.store(false);
	if (!client.Initialize())
	{
	
		cout << "Unable to connect to neither TWS nor IB Gateway!" << endl;
		Sleep(10000);
		clientValid.store(false);
	}
	else
	{
		messageProcess_ = new thread(&TheTradingMachine::IBInterfaceClient::messageProcess, this);
		while (!clientReady.load())
		{
			Sleep(10);
		}
		cout << "Successfully initialized IB Client." << endl;
	}
}

TheTradingMachine::IBInterfaceClient::~IBInterfaceClient()
{
	if (threadRunning.load())
	{
		//
		// Stop the thread
		//
		threadRunning.store(false);
		if (messageProcess_ != nullptr)
		{
			messageProcess_->join();
			delete messageProcess_;
			messageProcess_ = nullptr;
			client.disconnect();
		}
	}
}

void TheTradingMachine::IBInterfaceClient::requestRealTimeTicks(string ticker, function<void(const Tick&)> callback)
{
	client.requestRealTimeTicks(ticker, callback);
}

void TheTradingMachine::IBInterfaceClient::messageProcess(void)
{

	threadRunning.store(true);
	if (client.isConnected())
	{
		cout << "IB Message Processing Thread has started." << endl;
	}

	clientReady.store(true);
	while (client.isConnected() && threadRunning.load())
	{
		client.processMessages();
		Sleep(10);
	}

	clientValid.store(false);
}

