#include "SimpleMovingAverage.h"
#include <iostream>
#include <string>

class SimpleMovingAverage::SimpleMovingAverageImpl
{
public:
	IndicatorPoint<SimpleMovingAverage> computeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample);
	IndicatorPoint<SimpleMovingAverage> recomputeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample);
private:

};

IndicatorPoint<SimpleMovingAverage> SimpleMovingAverage::computeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample)
{
	return impl_->computeIndicatorPoint(sample);
}

IndicatorPoint<SimpleMovingAverage> SimpleMovingAverage::recomputeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample)
{
	return impl_->recomputeIndicatorPoint(sample);
}

IndicatorPoint<SimpleMovingAverage> SimpleMovingAverage::SimpleMovingAverageImpl::computeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample)
{
	std::cout << "compute" << std::endl;
	return IndicatorPoint<SimpleMovingAverage>();
}

IndicatorPoint<SimpleMovingAverage> SimpleMovingAverage::SimpleMovingAverageImpl::recomputeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample)
{
	std::cout << "recompute" << std::endl;
	return IndicatorPoint<SimpleMovingAverage>();
}
