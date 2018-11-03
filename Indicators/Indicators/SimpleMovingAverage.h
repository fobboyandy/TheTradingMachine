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

	using array_type = std::array<DataPoint, SIZE>;

	SimpleMovingAverage(int period);
	~SimpleMovingAverage();

	array_type computeIndicatorPoint(const DataPoint& sample);
	array_type recomputeIndicatorPoint(const DataPoint& sample);

private:
	class SimpleMovingAverageImpl;
	SimpleMovingAverageImpl* impl_;
};
