#pragma once
#include <vector>
#include <functional>
#include "../BaseAlgorithm/BaseAlgorithm/PlotData.h"

class CandleMaker
{
public:

	//
	// Given the streaming real time price snapshots and CandleMaker will 
	// aggregate the snapshots into candles of the desired time frame
	//
    CandleMaker(int timeFrameSeconds, bool rth = true);

	~CandleMaker();

	//
    // Given a new tick, updates the candle and the latest candle time. returns true for new candles.
    //
    bool updateCandle(const Tick& newTick, Bar& updatedCandle);

    //
    // getUpdatedCandleTime will return the updated time to the nearest timeFrame relative
    // to the last time updateCandle was called.
    //
    time_t getUpdatedCandleTime();

    void setRthOnly(bool rth);

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
    time_t candlePeriodCounter;

    //
    // Used to keep track of the time that the latest candle belongs to
    //
    time_t currentCandleTime;

    bool rthOnly;

private:
	//
	// Aggregates the current candle when a new price tick comes in. 
	// Update candle mins and max, open and close, and volume. 
	//
	void aggregateCandle(const Tick& newTick);


};
