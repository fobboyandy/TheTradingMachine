#pragma once

#include "Common.h"
#include "Indicator.h"
#include <ctime>

class INDICATORSDLL SimpleMovingAverage : Indicator<SimpleMovingAverage>
{
public:

	// indices for output datapoints for this indicator
	enum class DataPointIndex
	{
		MOVING_AVERAGE
	};

	SimpleMovingAverage(int period);
	~SimpleMovingAverage();

	std::vector<DataPoint> computeIndicatorPoint(const DataPoint& sample) override;
	std::vector<DataPoint> recomputeIndicatorPoint(const DataPoint& sample) override;

private:
	class SimpleMovingAverageImpl;
	SimpleMovingAverageImpl* impl_;
};
