#ifndef IPLOT_H
#define IPLOT_H

// base class for all plots
// interface class for storing base class ptrs for various
// indicator plots in a container

#include "../InteractiveBrokersClient/InteractiveBrokersApi/bar.h"
#include <ctime>

class IPlot
{
public:
    IPlot(){}
    virtual ~IPlot(){}

    // interface for updating the plot
    virtual void updatePlotNewCandle(const time_t candleTime, const Bar &candle) = 0;
    virtual void updatePlotReplaceCandle(const time_t candleTime, const Bar &candle) = 0;
    virtual void rescaleValueAxisAutofit() = 0;

};

#endif // IPLOT_H

