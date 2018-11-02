#pragma once

#include "Common.h"
#include "Indicator.h"
#include <ctime>

class INDICATORSDLL SimpleMovingAverage : Indicator<SimpleMovingAverage>
{
public:
	SimpleMovingAverage(int period);
	~SimpleMovingAverage();

	IndicatorPoint<SimpleMovingAverage> computeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample) override;
	IndicatorPoint<SimpleMovingAverage> recomputeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample) override;

private:
	class SimpleMovingAverageImpl;
	SimpleMovingAverageImpl* impl_;
};

// template specialization
template<>
struct IndicatorPoint<SimpleMovingAverage>
{
	time_t time;
	double value;
};

template<>
struct SamplePoint<SimpleMovingAverage>
{
	time_t time;
	double value;
};
