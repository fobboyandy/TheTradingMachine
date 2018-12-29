#pragma once

#include <memory>
#include <mutex>
#include <list>
#include "Common.h"

// plot data structure shared with the gui. plot data is stored in this format.
// the gui is provided an address to the plot data struct and notified upon 
// new data
struct PlotData
{
    std::mutex plotDataMtx;
    std::list<Tick> ticks;
    std::list<std::shared_ptr<Annotation::IAnnotation>> annotations;
};
