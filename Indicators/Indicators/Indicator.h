#pragma once

#include <vector>

template <typename DerivedIndicator>
struct IndicatorPoint;

template <typename DerivedIndicator>
struct SamplePoint;

template <typename T>
class Indicator
{
public:

	Indicator();
	virtual ~Indicator();
	virtual IndicatorPoint<T> createNewPoint(SamplePoint<T> sample) = 0;
	virtual IndicatorPoint<T> recomputePoint(SamplePoint<T> sample) = 0;

protected:

	// keep a history of sample and indicator points in order to compute the a new point. 
	// varies for different indicators. for example, sma for moving period of 5 requires 5 points
	// to output one point. when a new point comes in, the oldest point is moved out and the most
	// recent 5 points will be used to compute the next indicator point
	std::vector<SamplePoint<T>> _samplePointBuffer;
	std::vector<IndicatorPoint<T>> _indicatorPointBuffer;
};

template<class T>
inline Indicator<T>::Indicator()
{
}

template<class T>
inline Indicator<T>::~Indicator()
{
}
