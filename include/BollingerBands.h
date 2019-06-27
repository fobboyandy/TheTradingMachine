#pragma once

#include "Indicators.h"

class INDICATORSDLL BollingerBands
{
public:
	BollingerBands(unsigned int period, double scale);
	~BollingerBands();

	// indices for output datapoints for this indicator
	enum
	{
		UPPER,
		MIDDLE,
		LOWER,
		SIZE
	};

	using array_type = std::array<double, SIZE>;

	array_type computeIndicatorPoint(const double sample);
	array_type recomputeIndicatorPoint(const double sample);

private:
	class BollingerBandsImpl;
	BollingerBandsImpl* impl_;
};
