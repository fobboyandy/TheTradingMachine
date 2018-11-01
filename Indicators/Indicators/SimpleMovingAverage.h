#pragma once
#include "Indicator.h"

class SimpleMovingAverage : public Indicator<SimpleMovingAverage>
{
public:
	SimpleMovingAverage(int period);
	~SimpleMovingAverage();
};

// specialization for IP and SP for SMA
template <>
struct IndicatorPoint<SimpleMovingAverage>
{
	double key;
	double value;
};

template <>
struct SamplePoint<SimpleMovingAverage>
{
	double key;
	double value;
};