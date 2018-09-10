#include <iostream>
#include <ctime>
#include "CandleMaker.h"

#define NUM_SECONDS_DAY 86400
#define RTH_START 48600
#define RTH_END 72000


inline bool isRTH(time_t tickTime)
{
	time_t secondsPassed = tickTime % NUM_SECONDS_DAY;
	if (secondsPassed >= RTH_START && secondsPassed < RTH_END)
	{
		return true;
	}
	return false;
}

CandleMaker::CandleMaker(int timeFrameSeconds) :
	timeFrame(timeFrameSeconds),
	beginAggregation(false)
{
}

CandleMaker::~CandleMaker()
{
}

bool CandleMaker::getRthCandle(const Tick& newTick, Bar& newCandle)
{
	bool candleValid = false;
	//
	// check for RTH
	//
	if (!isRTH(newTick.time))
	{
		return candleValid;
	}
	
	time_t thisPeriodCounter = newTick.time / timeFrame;

	//
	// if thisPeriod > prevCandlePeriod, then it's a new minute
	// since it won't increment unless it reaches a new period
	//
	if (thisPeriodCounter > candlePeriodCounter)
	{
		candlePeriodCounter = thisPeriodCounter;

		//
		// Check if we are at the start of a new time period. For example,
		// if we started this price stream with time frame 1 minute at 9:00:30,
		// half of the first minute has passed. Therefore, we need to wait until 
		// 9:01:00 to begin since we lost the first 30 second of information.
		//
		if (beginAggregation)
		{
			newCandle = aggregatedCandle;
			candleValid = true;
		}
		else
		{
			beginAggregation = true;
		}
		//
		//Reset the candlebar after each time increment ends
		//
		aggregatedCandle.open = newTick.price;
		aggregatedCandle.low = newTick.price;
		aggregatedCandle.high = newTick.price;
		aggregatedCandle.close = newTick.price;
		aggregatedCandle.volume = newTick.size;
	}
	else
	{
		aggregateCandle(newTick);
	}

	return candleValid;
}

void CandleMaker::aggregateCandle(const Tick& newTick)
{
	aggregatedCandle.close = newTick.price;
	if (newTick.price < aggregatedCandle.low)
		aggregatedCandle.low = newTick.price;
	else if (newTick.price > aggregatedCandle.high)
		aggregatedCandle.high = newTick.price;

	aggregatedCandle.volume += newTick.size;

}
