#pragma once

#include "Indicators.h"

class INDICATORSDLL ExponentialMovingAverage
{
public:

	// indices for output datapoints for this indicator
	enum
	{
		EXPONENTIAL_MOVING_AVERAGE,
		SIZE
	};

	using array_type = std::array<double, SIZE>;

	ExponentialMovingAverage(int period);
	~ExponentialMovingAverage();

	array_type computeIndicatorPoint(const double sample);
	array_type recomputeIndicatorPoint(const double sample);

private:
	class ExponentialMovingAverageImpl;
	ExponentialMovingAverageImpl* impl_;
};
