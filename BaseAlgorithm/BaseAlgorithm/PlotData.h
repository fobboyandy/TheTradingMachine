#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <queue>
#include "Common.h"

// plot data structure shared with the gui. plot data is stored in this format.
// the gui is provided an address to the plot data struct and notified upon 
// new data
struct PlotData
{
	std::mutex plotDataMtx;
	std::vector<Tick> ticks;
    std::vector<std::shared_ptr<Annotation::IAnnotation>> annotations;
};
