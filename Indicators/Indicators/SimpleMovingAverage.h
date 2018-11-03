#pragma once

#include "Common.h"
#include <ctime>
#include <array>

class INDICATORSDLL SimpleMovingAverage
{
public:

	// indices for output datapoints for this indicator
	enum DataPointIndex
	{
		MOVING_AVERAGE,
		SIZE
	};

	SimpleMovingAverage(int period);
	~SimpleMovingAverage();

	std::array<DataPoint, SIZE> computeIndicatorPoint(const DataPoint& sample);
	std::array<DataPoint, SIZE> recomputeIndicatorPoint(const DataPoint& sample);

private:
	class SimpleMovingAverageImpl;
	SimpleMovingAverageImpl* impl_;
};
