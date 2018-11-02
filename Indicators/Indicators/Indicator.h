#pragma once

#include "Common.h"
#include <ctime>
#include <vector>

struct DataPoint
{
	time_t time;
	double value;
};


// the two virtual functions are free to be modified by the user to yield different results
// however, the input and output structure must stay the same in order to use dynamic linking.
// otherwise, the gui must be recompiled if the structs of indicators change
// return values of the indicators must be in vector in order to be general enough
// for GUI function to plot it at compile time using a templated function
// each derived indicator will contain enums for identifying the index of the corresponding
// output datapoint
template<typename T>
class Indicator
{
public:
    Indicator(){}
    virtual ~Indicator(){}

    // this computes an indicator point and permanently adds it to the chart
	virtual std::vector<DataPoint> computeIndicatorPoint(const DataPoint& sample) = 0;
	
	// for reflecting real time tick changes by removing the last computed point and 
	// recomputing with a new point. to increase efficiency, users should keep a small
	// buffer which keeps a minimum number of previous sample points to recompute the next 
	// indicator point instead of recalculating from the beginning.
	virtual std::vector<DataPoint> recomputeIndicatorPoint(const DataPoint& sample) = 0;
};
