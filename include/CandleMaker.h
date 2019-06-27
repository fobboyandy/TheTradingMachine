#pragma once
#include <vector>
#include <functional>
#include "../InteractiveBrokersApi/bar.h"
#include "../InteractiveBrokersApi/Tick.h"

#ifdef CandleMaker_EXPORTS
#define CANDLEMAKERDLL __declspec(dllexport)
#else
#define CANDLEMAKERDLL __declspec(dllimport)
#endif

struct Candlestick
{
	time_t time;
	double high;
	double low;
	double open;
	double close;
	double wap;
	long long volume;
	int count;
};

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
	// updates the CandleMaker by adding a tick
	//
	void addTick(const Tick& newTick);

	//
	// Retrieve candles
	//
	std::vector<Candlestick> getClosedCandles();
	Candlestick getCurrentCandle();

private:
	class CandleMakerImpl;
	CandleMakerImpl* impl_;

};
