#include <iostream>
#include <ctime>
#include "CandleMaker.h"

#define CTIME_MINUS_6_MAGIC 18000
#define SECONDS_IN_DAY 86400

using namespace std;

CandleMaker::CandleMaker(int timeFrameSeconds) :
	timeFrame(timeFrameSeconds),
	beginAggregation(false)
{
}

CandleMaker::~CandleMaker()
{
}

bool CandleMaker::getNewCandle(const Tick& newTick, Bar& newCandle)
{

	time_t thisPeriod = newTick.time - (newTick.time % timeFrame);

	//
	// if thisPeriod > prevCandlePeriod, then it's a new minute
	//
	if (thisPeriod > prevCandlePeriod)
	{
		prevCandlePeriod = thisPeriod;

		//
		// Check if we are at the start of a new time period. For example,
		// if we started this price stream with time frame 1 minute at 9:00:30,
		// half of the first minute has passed. Therefore, we need to wait until 
		// 9:01:00 to begin since we lost the first 30 second of information.
		//
		if (beginAggregation)
		{
			newCandle = aggregatedCandle;
			return true;
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

	

	return false;
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
