#pragma once
#include "Indicators.h"

class INDICATORSDLL SimpleMovingAverage
{
public:

	// indices for output datapoints for this indicator
	enum 
	{
		MOVING_AVERAGE,
		SIZE
	};

	using array_type = std::array<double, SIZE>;

	SimpleMovingAverage(int period);
	~SimpleMovingAverage();

	array_type computeIndicatorPoint(const double sample);
	array_type recomputeIndicatorPoint(const double sample);

private:
	class SimpleMovingAverageImpl;
	SimpleMovingAverageImpl* impl_;
};
