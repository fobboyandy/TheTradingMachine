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

CandleMaker::CandleMaker(int timeFrameSeconds, bool rth) :
	timeFrame(timeFrameSeconds),
    beginAggregation(false),
    rthOnly(rth)
{
}

CandleMaker::~CandleMaker()
{
}

bool CandleMaker::updateCandle(const Tick& newTick, Bar& updatedCandle)
{
    bool isNewCandle = false;

    //
    // check for RTH and also don't act on unreported ticks
    if ((rthOnly && !isRTH(newTick.time)) || newTick.attributes.unreported)
    {
        //return the last valid candle we saw
        updatedCandle = aggregatedCandle;
        return isNewCandle;
    }

    // check if the current time is in the middle of an
    // on going candle. For example, if we started this candle maker with time
    // frame 1 minute at 9:00:30, half of the first minute has passed. Therefore,
    // we need to wait until 9:01:00 to begin since we lost the first 30 second
    // of information.

    if(!beginAggregation)
    {
        if(newTick.time % timeFrame == 0)
        {
            beginAggregation = true;
        }
        else
        {
            return isNewCandle;
        }
    }

    time_t thisPeriodCounter = newTick.time / timeFrame;

    //
    // if thisPeriod > prevCandlePeriod, then it's a new candle
    // since thisPeriodCounter roll over after truncated division
    //
    if (thisPeriodCounter > candlePeriodCounter)
    {
        candlePeriodCounter = thisPeriodCounter;

        //
        // create a new candle
        //
        aggregatedCandle.open = newTick.price;
        aggregatedCandle.low = newTick.price;
        aggregatedCandle.high = newTick.price;
        aggregatedCandle.close = newTick.price;
        aggregatedCandle.volume = newTick.size;
        isNewCandle = true;
        currentCandleTime = newTick.time - (newTick.time % timeFrame); // candle time should align with the nearest timeframe
    }
    else
    {
        aggregateCandle(newTick);
    }

    // this is the updated candle
    updatedCandle = aggregatedCandle;

    return isNewCandle;
}

time_t CandleMaker::getUpdatedCandleTime()
{
    return currentCandleTime;
}

void CandleMaker::setRthOnly(bool rth)
{
    // we don't need to synchronize this because this is read/write only in the
    // gui thread
    rthOnly = rth;
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
