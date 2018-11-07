#include "RelativeStrengthIndex.h"

class RelativeStrengthIndex::RelativeStrengthIndexImpl
{
public:
	RelativeStrengthIndexImpl(unsigned int period);
	~RelativeStrengthIndexImpl();

	array_type computeIndicatorPoint(const double sample);
	array_type recomputeIndicatorPoint(const double sample);
private:
	const unsigned int period_;
	double pVal_;

	// previous s up and s down. gets replaced by current s up and down for every new value (candle)
	// in computeIndicatorPoint
	double pSUp_;
	double pSDown_;

	double cVal_;

	// current s up and s down to hold temporary rsi calculations. gets replaced every time an update
	// value (candle) comes in recomputeIndicatorPoint;
	double cSUp_;
	double cSDown_;

	// keeps track of the num of inputs until
	// num inputs reaches period
	unsigned int size_;

};

RelativeStrengthIndex::RelativeStrengthIndex(unsigned int period) :
	impl_(new RelativeStrengthIndexImpl(period))
{
}

RelativeStrengthIndex::~RelativeStrengthIndex()
{
	delete impl_;
}

RelativeStrengthIndex::array_type RelativeStrengthIndex::computeIndicatorPoint(const double sample)
{
	return impl_->computeIndicatorPoint(sample);
}
RelativeStrengthIndex::array_type RelativeStrengthIndex::recomputeIndicatorPoint(const double sample)
{
	return impl_->recomputeIndicatorPoint(sample);
}
RelativeStrengthIndex::RelativeStrengthIndexImpl::RelativeStrengthIndexImpl(unsigned int period) :
	period_(period)
{
	size_ = 0;
	pVal_ = cVal_ = 0;
	pSUp_ = cSUp_ = 0;
	pSDown_ = cSDown_ = 0;
}

RelativeStrengthIndex::RelativeStrengthIndexImpl::~RelativeStrengthIndexImpl()
{
}

RelativeStrengthIndex::array_type RelativeStrengthIndex::RelativeStrengthIndexImpl::computeIndicatorPoint(const double sample)
{
	array_type result;

	// last modified values get stored into the p values;
	pVal_ = cVal_;
	pSUp_ = cSUp_;
	pSDown_ = cSDown_;

	// account for number of inputs for when num of inputs
	// hasn't reached period_, we output results using current 
	// size as the period
	if (size_ < period_)
		++size_;

	double up, down;
	sample > pVal_ ? up = sample - pVal_ : up = 0;
	sample < pVal_ ? down = pVal_ - sample : down = 0;

	//cVal is now the newest value.
	cVal_ = sample;

	// calculate the newest current sup and sdown values
	cSUp_ = (size_ - 1) * pSUp_ / size_ + up / size_;
	cSDown_ = (size_ - 1) * pSDown_ / size_ + down / size_;

	return array_type{ 100 - 100 / (1 + cSUp_ / cSDown_) };
}

RelativeStrengthIndex::array_type RelativeStrengthIndex::RelativeStrengthIndexImpl::recomputeIndicatorPoint(const double sample)
{
	array_type result;

	double up, down;

	// compare with pVal_ to assign up and down. we compute relative
	// to the previous input
	sample > pVal_ ? up = sample - pVal_ : up = 0;
	sample < pVal_ ? down = pVal_ - sample : down = 0;

	//cVal is now the newest value.
	cVal_ = sample;

	// calculate the current sup and sdown values and store them.
	// they get transferred to p sup and sdown when new values in come
	cSUp_ = (size_ - 1) * pSUp_ / size_ + up / size_;
	cSDown_ = (size_ - 1) * pSDown_ / size_ + down / size_;

	return array_type{ 100 - 100 / (1 + cSUp_ / cSDown_) };
}

