#include "SimpleMovingAverage.h"
#include <deque>

class SimpleMovingAverage::SimpleMovingAverageImpl
{
public:
	SimpleMovingAverageImpl(int period);
	~SimpleMovingAverageImpl();

	SimpleMovingAverage::array_type computeIndicatorPoint(const double sample);
	SimpleMovingAverage::array_type recomputeIndicatorPoint(const double sample);
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
	delete impl_;
}

SimpleMovingAverage::array_type SimpleMovingAverage::computeIndicatorPoint(const double sample)
{
	return impl_->computeIndicatorPoint(sample);
}

SimpleMovingAverage::array_type SimpleMovingAverage::recomputeIndicatorPoint(const double sample)
{
	return impl_->recomputeIndicatorPoint(sample);
}

SimpleMovingAverage::SimpleMovingAverageImpl::SimpleMovingAverageImpl(int period):
	period_(period),
	sum_(0)
{
}

SimpleMovingAverage::SimpleMovingAverageImpl::~SimpleMovingAverageImpl()
{
}

SimpleMovingAverage::array_type SimpleMovingAverage::SimpleMovingAverageImpl::computeIndicatorPoint(const double sample)
{
	window_.push_front(sample);
	sum_ += sample;
	
	// we only need to start removing from the back of window_
	// when the window_ has grown to the size > period_. 
	// otherwise, we simply take the average until the
	// window_ reaches a constant size. this way, we begin
	// averaging with the first received point
	if (window_.size() > period_)
	{
		sum_ -= window_.back();
		window_.pop_back();
	}

	return array_type{sum_ / window_.size()};
}

SimpleMovingAverage::array_type SimpleMovingAverage::SimpleMovingAverageImpl::recomputeIndicatorPoint(const double sample)
{

	// recompute the current moving average by replacing the latest point with sample

	sum_ -= window_.front();

	//replace the front with the current value
	window_.front() = sample;

	sum_ += sample;

	return array_type{ sum_ / window_.size()};

}
