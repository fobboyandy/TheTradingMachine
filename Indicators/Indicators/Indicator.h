#pragma once

#include "Common.h"

template <typename DerivedIndicator>
struct IndicatorPoint;

template <typename DerivedIndicator>
struct SamplePoint;

// the two virtual functions are free to be modified by the user to yield different results
// however, the input and output structure must stay the same in order to use dynamic linking.
// otherwise, the gui must be recompiled if the structs of indicators change
template<typename T>
class Indicator
{
public:
	// this computes an indicator point and permanently adds it to the chart
	virtual IndicatorPoint<T> computeIndicatorPoint(const SamplePoint<T>& sample) = 0;
	
	// for reflecting real time tick changes by removing the last computed point and 
	// recomputing with a new point. to increase efficiency, users should keep a small
	// buffer which keeps a minimum number of previous sample points to recompute the next 
	// indicator point instead of recalculating from the beginning.
	virtual IndicatorPoint<T> recomputeIndicatorPoint(const SamplePoint<T>& sample) = 0;
};