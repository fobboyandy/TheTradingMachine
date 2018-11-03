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
    virtual void updatePlotAdd(const time_t candleTime, double value) = 0;
    virtual void updatePlotReplace(const time_t candleTime, double value) = 0;
    virtual void rescaleValueAxisAutofit() = 0;

};

#endif // IPLOT_H

