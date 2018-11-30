#include "BollingerBands.h"
#include <list>


class BollingerBands::BollingerBandsImpl
{
public:
	BollingerBandsImpl(unsigned int period, double scale);
	~BollingerBandsImpl();

	array_type computeIndicatorPoint(const double sample);
	array_type recomputeIndicatorPoint(const double sample);
private:
	const unsigned int period_;
	const double scale_;
	std::list<double> window_;
	double sum_;
};


BollingerBands::BollingerBands(unsigned int period, double scale) :
	impl_(new BollingerBandsImpl(period, scale))
{
}

BollingerBands::~BollingerBands()
{
	delete impl_;
}

BollingerBands::array_type BollingerBands::computeIndicatorPoint(const double sample)
{
	return impl_->computeIndicatorPoint(sample);
}

BollingerBands::array_type BollingerBands::recomputeIndicatorPoint(const double sample)
{
	return impl_->recomputeIndicatorPoint(sample);
}

BollingerBands::BollingerBandsImpl::BollingerBandsImpl(unsigned int period, double scale) :
	period_(period),
	scale_(scale)
{
	sum_ = 0;
}

BollingerBands::BollingerBandsImpl::~BollingerBandsImpl()
{
}

BollingerBands::array_type BollingerBands::BollingerBandsImpl::computeIndicatorPoint(const double sample)
{
	array_type result;

	window_.push_front(sample);
	sum_ += sample;

	// adjust sum and window_ size fixed
	if (window_.size() > period_)
	{
		sum_ -= window_.back();
		window_.pop_back();
	}

	auto avg = sum_ / window_.size();

	result[MIDDLE] = avg;

	double offset = 0;
	// calculating stdDev
	for (const auto& val : window_)
	{
		offset += pow(val - avg, 2);
	}
	offset /= window_.size();
	offset = scale_ * std::sqrt(offset);

	result[LOWER] = avg - offset;
	result[UPPER] = avg + offset;

	return result;
}

BollingerBands::array_type BollingerBands::BollingerBandsImpl::recomputeIndicatorPoint(const double sample)
{
	array_type result;

	//replace the front value and recompute
	sum_ -= window_.front();
	window_.front() = sample;
	sum_ += sample;

	auto avg = sum_ / window_.size();

	result[MIDDLE] = avg;

	double offset = 0;
	// calculating stdDev
	for (const auto& val : window_)
	{
		offset += pow(val - avg, 2);
	}
	offset /= window_.size();
	offset = scale_ * std::sqrt(offset);

	result[LOWER] = avg - offset;
	result[UPPER] = avg + offset;

	return result;
}