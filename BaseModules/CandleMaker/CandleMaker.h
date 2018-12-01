#pragma once
#include <vector>
#include <functional>
#include "../InteractiveBrokersApi/bar.h"
#include "../InteractiveBrokersApi/Tick.h"

#ifdef CANDLEMAKER_EXPORTS
#define CANDLEMAKERDLL __declspec(dllexport)
#else
#define CANDLEMAKERDLL __declspec(dllimport)
#endif

class CANDLEMAKERDLL CandleMaker
{
public:
	//
	// Given the streaming real time price snapshots and CandleMaker will 
	// aggregate the snapshots into candles of the desired time frame
	//
    CandleMaker(int timeFrameSeconds);
	~CandleMaker();

	//
    // Given a new tick, updates the candle and the latest candle time. returns true for new candles.
    //
    bool updateCandle(const Tick& newTick, Bar& updatedCandle, time_t& updatedTime, bool& isNewCandle);

	// return the last known values
    time_t getTime() const;
	const Bar& getCandle() const;

private:
	class CandleMakerImpl;
	CandleMakerImpl* impl_;

};
