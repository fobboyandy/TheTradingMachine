#pragma once

//
// Each indicator is created as a dynamic shared library. We will use the indicators in both the GUI charting
// as well as in user defined algorithms. By making them dynamically linked libraries, users who want to modify
// an indicator will be able to have it reflect on the plotting in the GUI. The purpose of The Trading Machine
// is to assist the process of creating and testing strategies by visualizing its activities. Therefore, it makes 
// sense that if a user built his strategy upon a common indicator and wishes to modify the indicators' computation, it 
// would reflect the same modified indicator on the charts provided that the input and output data structs 
// (IndicatorPoint and SamplePoint) remain the same. 
//

#include <array>

#ifdef Indicators_EXPORTS
#define INDICATORSDLL __declspec(dllexport)
#else
#define INDICATORSDLL __declspec(dllimport)
#endif