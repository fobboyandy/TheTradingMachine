#pragma once

#include <iostream>
#include <functional>
#include "CandleMaker.h"
#include "../IBInterface/IBInterface.h"
#include "TheTradingMachine.h"

#define NUM_SECONDS_DAY 86400
#define RTH_SECONDS 48600
#define RTH_START 48600
#define RTH_END 72000

class SupportBreakShort : public TheTradingMachine
{
public:
	explicit SupportBreakShort(std::string input);
	void tickHandler(const Tick& tick);
	//
	// Check the openPositions for the top most position. The positions are 
	// sorted by target prices since the highest target price are the positions
	// that need to be closed first. 
	//
	void coverTrade();
	void shortTrade();

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
	Tick lastTick;

	Bar prevBar;
	Dir prevDir;
	Strength previousStrength;

	Bar prevSupportBar;
	Bar prevResistanceBar;

	std::fstream logFile;

	struct Position
	{
		double entry;
		time_t entryTime;
		double target;
		double stoploss;
		int size;
	};

	std::list<Position> openPositions;
	double profit;
};
