#pragma once
#include "Common.h"

class INDICATORSDLL RelativeStrengthIndex
{
public:
	RelativeStrengthIndex(unsigned int period);
	~RelativeStrengthIndex();

	enum
	{
		RSI,
		SIZE
	};

	using array_type = std::array<double, SIZE>;
	array_type computeIndicatorPoint(const double sample);
	array_type recomputeIndicatorPoint(const double sample);

private:
	class RelativeStrengthIndexImpl;
	RelativeStrengthIndexImpl* impl_;
};
