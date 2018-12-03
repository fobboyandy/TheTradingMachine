#pragma once

#pragma comment(lib, "../../BaseModules/x64/Debug/BaseAlgorithm.lib")
#pragma comment(lib, "../../BaseModules/x64/Debug/CandleMaker.lib")
#pragma comment(lib, "../../BaseModules/x64/Debug/Indicators.lib")

#include "../../BaseModules/BaseAlgorithm/BaseAlgorithm.h"
#include "../../BaseModules/CandleMaker/CandleMaker.h"
#include "../../BaseModules/Indicators/SimpleMovingAverage.h"
#include "../../BaseModules/Indicators/ExponentialMovingAverage.h"
#include <vector>

class SupportResistanceFinder : public BaseAlgorithm
{
public:
	SupportResistanceFinder(ALGORITHM_ARGS);
	~SupportResistanceFinder();

private:
	CandleMaker candleMaker_;

	ExponentialMovingAverage candlesEma_;
	ExponentialMovingAverage emaEma_;

	Bar pCandle;
	std::vector<Bar> candles;
	std::vector<double> candleEmaPoints;
	std::vector<double> emaEmaPoints;
	double pCandleEma;
	double pEmaEma;

	int numPoints = 0;

	time_t pCandleTime;

	virtual void tickHandler(const Tick& tick) override;

	template<typename T>
	bool isWithin(const T& val, const T& lower, const T& upper)
	{
		return (val >= lower && val <= upper);
	}

	PositionId posId = -1;
	int posType = -1; // 0 for long 1 for short


};