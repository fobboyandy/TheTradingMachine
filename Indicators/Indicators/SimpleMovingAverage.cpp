#include "SimpleMovingAverage.h"
#include <deque>

class SimpleMovingAverage::SimpleMovingAverageImpl
{
public:
	SimpleMovingAverageImpl(int period);
	~SimpleMovingAverageImpl();

	IndicatorPoint<SimpleMovingAverage> computeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample);
	IndicatorPoint<SimpleMovingAverage> recomputeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample);
private:
	const int period_;
	double sum_;

	// this queue is the size of the period. It holds the values of the history. When a new value is added it is pushed into the queue and
	// the value is added to the sum. At the same time, the oldest value is popped from the back and subtracted from the sum.
	std::deque<double> window_;
};

SimpleMovingAverage::SimpleMovingAverage(int period):
	impl_(new SimpleMovingAverageImpl(period))
{
}

SimpleMovingAverage::~SimpleMovingAverage()
{
	if (impl_ != nullptr)
	{
		delete impl_;
	}
}

IndicatorPoint<SimpleMovingAverage> SimpleMovingAverage::computeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample)
{

	return impl_->computeIndicatorPoint(sample);
}

IndicatorPoint<SimpleMovingAverage> SimpleMovingAverage::recomputeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample)
{
	return impl_->recomputeIndicatorPoint(sample);
}

SimpleMovingAverage::SimpleMovingAverageImpl::SimpleMovingAverageImpl(int period):
	period_(period),
	sum_(0)
{
	// initialize a queue of 0's with size == period
	for (int i = 0; i < period_; i++)
	{
		window_.push_front(0);
	}
}

SimpleMovingAverage::SimpleMovingAverageImpl::~SimpleMovingAverageImpl()
{
}

IndicatorPoint<SimpleMovingAverage> SimpleMovingAverage::SimpleMovingAverageImpl::computeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample)
{
	sum_ -= window_.back();
	window_.pop_back();

	sum_ += sample.value;
	window_.push_front(sample.value);

	return IndicatorPoint<SimpleMovingAverage>{sample.time, sum_ / period_};
}

IndicatorPoint<SimpleMovingAverage> SimpleMovingAverage::SimpleMovingAverageImpl::recomputeIndicatorPoint(const SamplePoint<SimpleMovingAverage>& sample)
{

	// recompute the current moving average by replacing the latest point with sample

	sum_ -= window_.front();

	//replace the front with the current value
	window_.front() = sample.value;

	sum_ += sample.value;

	return IndicatorPoint<SimpleMovingAverage>{sample.time, sum_ / period_};
}
