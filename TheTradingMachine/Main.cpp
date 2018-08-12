/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#include "StdAfx.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <functional>
#include <queue>
#include "CandleMaker.h"
#include "IBInterface.h"
#include "TheTradingMachine.h"

#define SIM 1

#define NUM_SECONDS_DAY 86400
#define RTH_SECONDS 48600


class TickRecorder : public TheTradingMachine
{
public:
	TickRecorder(string t) : TheTradingMachine(t), ticker(t)
	{
		if (t.find(".tickdat") != string::npos)
		{
			throw std::invalid_argument("It is already a file. Aborting...");
			return;
		}

		auto timenow = time(nullptr);
		string s = string(ctime(&timenow));

		//remove the newline
		string filename = s.substr(4, 6) + ticker + ".tickdat";
		tickoutput.open(filename, ios::trunc | ios::out);

		std::cout << "requesting ticks for " << ticker << std::endl;
		requestTicks([this](const Tick& tick) {this->tickHandler(tick); });
	}
	~TickRecorder()
	{
		tickoutput.close();
	}
	void tickHandler(const Tick& tick)
	{
		string s = ctime(&tick.time);
		tickoutput << tick.tickType << ',';
		tickoutput << tick.time << ",";
		tickoutput << s.substr(0, s.length() - 1) << ",";
		tickoutput << tick.price << ',';
		tickoutput << tick.size << ',';
		tickoutput << (int)tick.attributes.canAutoExecute << ',';
		tickoutput << (int)tick.attributes.pastLimit << ',';
		tickoutput << (int)tick.attributes.preOpen << ',';
		tickoutput << (int)tick.attributes.unreported << ',';
		tickoutput << (int)tick.attributes.bidPastLow << ',';
		tickoutput << (int)tick.attributes.askPastHigh << ',';
		tickoutput << tick.exchange << std::endl;

		std::cout << ticker << "\t" << tick.price << '\t' << tick.size << std::endl;
	}
	
private:
	fstream tickoutput;
	string ticker;

};

class SupportBreakShort : public TheTradingMachine
{
public:
	explicit SupportBreakShort(string input) :
		TheTradingMachine(input),
		minuteBarMaker(60),
		prevDir(UNDEFINED),
		previousStrength(NONE)
	{
		//
		// TheTradingMachine handles the source of the ticks
		//
		requestTicks([this](const Tick& tick) {this->tickHandler(tick); });
	}

	void tickHandler(const Tick& tick)
	{

		Bar minuteBar;
		lastPrice = tick.price;

		//
		// Cover trade based on real time prices. Only used in simulation. We use a stoploss
		// for real trades.
		//
		coverTrade();

		//
		// Enter new trade for new candles
		//
		if (minuteBarMaker.getRthCandle(tick, minuteBar))
		{
			//
			// price increase, check previous direction and mark 
			// indicators
			//
			if (minuteBar.close > prevBar.close)
			{
				if (prevDir == DOWN)
				{
					previousStrength = SUPPORT;
					prevSupportBar = prevBar;
				}
				prevDir = UP;
			}
			//
			// Price decrease. Check if short
			//
			else if (minuteBar.close < prevBar.close)
			{
				if (prevDir == UP)
				{
					previousStrength = RESISTANCE;
					prevResistanceBar = prevBar;
				}
				prevDir = DOWN;
				shortTrade();
			}
			prevBar = minuteBar;
		}

		//
		// Check if any positions need to be closed. Ideally this should be handled 
		// by the broker by sending conditions but we need this here to be able to 
		// run backtests.
		//

		coverTrade();

	}

	//
	// Check the openPositions for the top most position. The positions are 
	// sorted by target prices since the highest target price are the positions
	// that need to be closed first. 
	//
	void coverTrade() 
	{

	}

	void shortTrade()
	{
		//
		// If the price falls past the previous support and the risk reward is still worth it,
		// size and short the stock
		//
		if (lastPrice < prevSupportBar.close)
		{
			double targetGain = prevResistanceBar.close - prevSupportBar.close;
			double error = lastPrice - prevSupportBar.close;
			int toleranceFactor = 2;

			if (abs(error) < abs(targetGain / toleranceFactor))
			{
				std::cout << "short at:" << lastPrice << std::endl;
				double targetPrice = prevSupportBar.close - targetGain;
				double stoploss = prevResistanceBar.close;

				Position newPosition;
				newPosition.entry = lastPrice;
				newPosition.stoploss = stoploss;
				newPosition.target = targetPrice;
				newPosition.size = static_cast<int>(100 / abs(newPosition.entry - newPosition.stoploss));
				openPositions.push(newPosition);
			}
		}
	}

private:
	enum Dir
	{
		UP,
		DOWN,
		UNDEFINED
	};
	enum Strength
	{
		SUPPORT,
		RESISTANCE,
		NONE,
	};

	//
	// Minute candles
	//
	CandleMaker minuteBarMaker;


	//
	// Trading data
	//
	double lastPrice;

	Bar prevBar;
	Dir prevDir;
	Strength previousStrength;

	Bar prevSupportBar;
	Bar prevResistanceBar;

	fstream logFile;

	struct Position
	{
		bool operator<(const Position& other) const
		{
			return this->target < other.target;
		}
		double entry;
		double target;
		double stoploss;
		int size;
	};

	std::priority_queue <Position> openPositions;
};

#if SIM
/* IMPORTANT: always use your paper trading account. The code below will submit orders as part of the demonstration. */
/* IB will not be responsible for accidental executions on your live account. */
/* Any stock or option symbols displayed are for illustrative purposes only and are not intended to portray a recommendation. */
/* Before contacting our API support team please refer to the available documentation. */
int main(int argc, char** argv)
{

	//TickRecorder record1("AMD");
	//TickRecorder record2("NVDA");
	//TickRecorder record3("AMZN");

	SupportBreakShort test("D:\\Users\\fobboyandy\\Desktop\\TheTradingMachine\\x64\\Debug\\Jul 13AMDTickData.tickdat");

	while (1)
	{
		Sleep(10);
	}
}
#else
int main()
{
	vector<vector<string>> csvMax;
	CsvReader("D:\\Users\\fobboyandy\\Desktop\\TheTradingMachine\\outputfiles\\Jul10AMDTickData.csv", ref(csvMax));
	//

	vector<double> prices;

	for (auto&i : csvMax)
		prices.push_back(stod(i[2]));
	int windowSize = 5;
	auto& average = RunningAverage(prices, windowSize);
	fstream runningaverageout("output.txt", ios::out | ios::trunc);
	for (auto&i : average)
	{
		runningaverageout << i << std::endl;
	}

}

#endif