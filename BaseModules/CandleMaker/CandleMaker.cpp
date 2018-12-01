#include <ctime>
#include "CandleMaker.h"

class CandleMaker::CandleMakerImpl
{
public:
	//
	// Given the streaming real time price snapshots and CandleMaker will 
	// aggregate the snapshots into candles of the desired time frame
	//
	CandleMakerImpl(int timeFrameSeconds);
	~CandleMakerImpl();

	//
	// Given a new tick, updates the candle and the latest candle time. returns true if valid.
	//
	bool updateCandle(const Tick& newTick, Bar& updatedCandle, time_t& updatedTime, bool& isNewCandle);

	// return the last known values
	time_t getTime() const;
	const Bar& getCandle() const;

private:
	//
	// Number of seconds per candle. If timeFrame = 1 second, then we update a 
	// candle at absolute time at increments of 1 second. eg 8:30:00, 8:30:01, 
	// 8:30:02, etc... If timeFrame = 5, then we update at 8:30:00, 8:30:05,
	// 8:30:10. If we began requested a stream in the middle of an increment, 
	// we will start the actual recording at the next whole time unit. For example,
	// if we requested a CandleMaker at 10:00:33 with timeframe of 1min, then we wait
	// until 10:01:00 to begin recording the first candle since we lost some price data
	// for the current minute candle
	//
	const int timeFrame;

	//
	// Variable to indicate whether price stream has begun
	//
	bool beginAggregation;

	//
	// Aggregated candle since the beginning of the current time increment
	//
	Bar aggregatedCandle;

	//
	// Used to keep track of periods of elapsed timeframes to prevent
	// ticks within the same timeframe period to trigger a new candle
	//
	time_t periodIndex;

	//
	// Used to keep track of the time that the latest candle belongs to
	//
	time_t currentCandleTime;

private:
	//
	// Aggregates the current candle when a new price tick comes in. 
	// Update candle mins and max, open and close, and volume. 
	//
	void aggregateCandle(const Tick& newTick);

};


CandleMaker::CandleMaker(int timeFrameSeconds) :
	impl_(new CandleMakerImpl(timeFrameSeconds))
{
}

CandleMaker::~CandleMaker()
{
	delete impl_;
}

bool CandleMaker::updateCandle(const Tick& newTick, Bar& updatedCandle, time_t& updatedTime, bool& isNewCandle)
{
	return impl_->updateCandle(newTick, updatedCandle, updatedTime, isNewCandle);
}

time_t CandleMaker::getTime() const
{
	return impl_->getTime();
}

const Bar& CandleMaker::getCandle() const
{
    return impl_->getCandle();
}

CandleMaker::CandleMakerImpl::CandleMakerImpl(int timeFrameSeconds) :
	timeFrame(timeFrameSeconds),
	beginAggregation(false)
{
}

CandleMaker::CandleMakerImpl::~CandleMakerImpl()
{
}

bool CandleMaker::CandleMakerImpl::updateCandle(const Tick& newTick, Bar& updatedCandle, time_t& updatedTime, bool& isNewCandle)
{
	isNewCandle = false;
	time_t thisPeriodIndex = newTick.time / timeFrame;

	//check if the current time is in the middle of an
	//on going candle. For example, if we started this candle maker with time
	//frame 1 minute at 9:00:30, half of the first minute has passed. Therefore,
	//we need to wait until tick of the next period to begin since we lost the first 30 second
	//of information. we know that we are in the beginning of a new candle once
	//thisPeriodIndex is greater than the previous periodIndex.
	if(!beginAggregation)
	{
		if(thisPeriodIndex > periodIndex)
		{
			beginAggregation = true;
		}
		else
		{
			return false;
		}

		periodIndex = thisPeriodIndex;
	}
	
	 //if thisPeriodIndex > periodIndex, then it's a new candle
	 //since thisPeriodCounter roll over after truncated division
	if (thisPeriodIndex > periodIndex)
	{
		periodIndex = thisPeriodIndex;

		//create a new candle
		
		aggregatedCandle.open = newTick.price;
		aggregatedCandle.low = newTick.price;
		aggregatedCandle.high = newTick.price;
		aggregatedCandle.close = newTick.price;
		aggregatedCandle.volume = newTick.size;
		currentCandleTime = newTick.time - (newTick.time % timeFrame); // candle time should align with the nearest timeframe
		isNewCandle = true;
	}
	else
	{
		aggregateCandle(newTick);
	}

	// this is the updated candle
	updatedCandle = aggregatedCandle;
	updatedTime = currentCandleTime;

	return true;
}

time_t CandleMaker::CandleMakerImpl::getTime() const
{
	return currentCandleTime;
}

const Bar& CandleMaker::CandleMakerImpl::getCandle() const
{
	return aggregatedCandle;
}

void CandleMaker::CandleMakerImpl::aggregateCandle(const Tick& newTick)
{
	aggregatedCandle.close = newTick.price;
	if (newTick.price < aggregatedCandle.low)
		aggregatedCandle.low = newTick.price;
	else if (newTick.price > aggregatedCandle.high)
		aggregatedCandle.high = newTick.price;

	aggregatedCandle.volume += newTick.size;
}

