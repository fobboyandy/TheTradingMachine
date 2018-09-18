#pragma once
#include <mutex>
#include <condition_variable>
#include <vector>
#include "TheTradingMachine.h"

// Plot data stored for the GUI to plot. Data is stored into vectors and it is
// up to the GUI to keep manage the data. GUI must redefine the struct. This header
// file is not meant to be shared.
namespace SupportBreakShortPlotData
{
	struct PlotData
	{
		std::mutex plotDataMtx;
		std::condition_variable plotDataCv;
		std::vector<Tick>* ticks;
		std::vector<std::string>* action;
	};
}