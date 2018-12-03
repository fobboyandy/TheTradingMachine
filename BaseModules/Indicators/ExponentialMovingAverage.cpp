#include "ExponentialMovingAverage.h"

class ExponentialMovingAverage::ExponentialMovingAverageImpl
{
public:
	ExponentialMovingAverageImpl(int period);
	~ExponentialMovingAverageImpl();

	array_type computeIndicatorPoint(const double sample);
	array_type recomputeIndicatorPoint(const double sample);

private:
	const double scale_;
	double pEma_;
	double cEma_;
};


ExponentialMovingAverage::ExponentialMovingAverage(int period):
	impl_(new ExponentialMovingAverageImpl(period))
{
}

ExponentialMovingAverage::~ExponentialMovingAverage()
{
	delete impl_;
}

ExponentialMovingAverage::array_type ExponentialMovingAverage::computeIndicatorPoint(const double sample)
{
	return impl_->computeIndicatorPoint(sample);
}

ExponentialMovingAverage::array_type ExponentialMovingAverage::recomputeIndicatorPoint(const double sample)
{
	return impl_->recomputeIndicatorPoint(sample);
}

ExponentialMovingAverage::ExponentialMovingAverageImpl::ExponentialMovingAverageImpl(int period):
	scale_(static_cast<double>(2) / ( period + 1))
{
	cEma_ = 0;
	pEma_ = cEma_;
}

ExponentialMovingAverage::ExponentialMovingAverageImpl::~ExponentialMovingAverageImpl()
{
}

ExponentialMovingAverage::array_type ExponentialMovingAverage::ExponentialMovingAverageImpl::computeIndicatorPoint(const double sample)
{
	// save the current cma as the previous 
	pEma_ = cEma_;

	// first point of ema is the input itself
	if (cEma_ == 0)
	{
		cEma_ = sample;
	}
	else
	{
		cEma_ = (1 - scale_) * (pEma_)+(scale_ * sample);
	}

	return {cEma_};
}

ExponentialMovingAverage::array_type ExponentialMovingAverage::ExponentialMovingAverageImpl::recomputeIndicatorPoint(const double sample)
{
	// first point of the ema is the input value itself
	if (pEma_ == 0)
	{
		return { sample };
	}
	return {(1 - scale_) * (pEma_) + scale_ * sample};
}
