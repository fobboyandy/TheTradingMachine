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
	// updates the Candle maker by adding a tick
	//
	void addTick(const Tick& newTick);
	
	//
	// Has outstanding closed candles. Caller can retrieve them by calling getClosedCandles().
	// Calling getClosedCanddles will clear the candle buffer.
	//
	bool hasClosedCandles();

	//
	// Retrieve candles
	//
	std::vector<Candlestick> getClosedCandles();
	Candlestick getCurrentCandle();

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
	bool periodIndexInit;

	//
	// Aggregated candle since the beginning of the current time increment
	//
	Candlestick aggregatedCandle;

	//
	// Used to keep track of periods of elapsed timeframes to prevent
	// ticks within the same timeframe period to trigger a new candle
	//
	time_t periodIndex;

private:
	//
	// Aggregates the current candle when a new price tick comes in. 
	// Update candle mins and max, open and close, and volume. 
	//
	void aggregateCandle(const Tick& newTick);

	std::vector<Candlestick> closedCandles;

};


CandleMaker::CandleMaker(int timeFrameSeconds) :
	impl_(new CandleMakerImpl(timeFrameSeconds))
{
}

CandleMaker::~CandleMaker()
{
	delete impl_;
}

void CandleMaker::addTick(const Tick & newTick)
{
	impl_->addTick(newTick);
}

std::vector<Candlestick> CandleMaker::getClosedCandles()
{
	return impl_->getClosedCandles();
}

Candlestick CandleMaker::getCurrentCandle()
{
	return impl_->getCurrentCandle();
}

CandleMaker::CandleMakerImpl::CandleMakerImpl(int timeFrameSeconds) :
	timeFrame(timeFrameSeconds)
{
	beginAggregation = false;
	periodIndexInit = false;
}

CandleMaker::CandleMakerImpl::~CandleMakerImpl()
{
}

void CandleMaker::CandleMakerImpl::addTick(const Tick & newTick)
{
	time_t thisPeriodIndex = newTick.time / timeFrame;

	// initialize the period index with the first tick
	// we use periodIndex to align the start time with the 
	// next earliest time frame. for example, at 09:30:30,
	// with a timeframe 60 seconds, the next time frame
	// start would be at 9:31:00. We cannot use the first 
	// tick because there is no way to tell whether if it was
	// truly the first tick of the starting time frame. we always
	// need to wait for the start of the next period
	if (!periodIndexInit)
	{
		periodIndexInit = true;
		periodIndex = thisPeriodIndex;
	}
	else
	{
		if (thisPeriodIndex > periodIndex)
		{
			// if we have began aggregation, then
			// the current aggregatedCandle is a valid
			// closing candle 
			if (beginAggregation)
			{
				//push the current candle to the closed candles
				closedCandles.push_back(aggregatedCandle);
			}
			//create a new candle
			aggregatedCandle.open = newTick.price;
			aggregatedCandle.low = newTick.price;
			aggregatedCandle.high = newTick.price;
			aggregatedCandle.close = newTick.price;
			aggregatedCandle.volume = newTick.size;
			aggregatedCandle.time = newTick.time - (newTick.time % timeFrame); // candle time should align with the nearest timeframe

			beginAggregation = true;

			// update periodIndex
			periodIndex = thisPeriodIndex;
		}
		else
		{
			aggregateCandle(newTick);
		}
	}
}

bool CandleMaker::CandleMakerImpl::hasClosedCandles()
{
	return closedCandles.size() > 0;
}

std::vector<Candlestick> CandleMaker::CandleMakerImpl::getClosedCandles()
{
	auto retClosedCandles = std::move(closedCandles);
	closedCandles.clear();
	return retClosedCandles;
}

Candlestick CandleMaker::CandleMakerImpl::getCurrentCandle()
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

