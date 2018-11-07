#ifndef VOLUMEPLOT_H
#define VOLUMEPLOT_H

#include "qcustomplot.h"
#include "../InteractiveBrokersApi/bar.h"
#include "iplot.h"
#include <unordered_map>
#include <ctime>

class VolumePlot
{
public:
    VolumePlot(QCPAxisRect& axisRect);
    ~VolumePlot() ;
    void updatePlotAdd(const time_t candleTime, const Bar &candle);
    void updatePlotReplace(const time_t candleTime, const Bar &candle);
    void rescaleValueAxisAutofit();
    void addIndicator(IndicatorType indicatorType, std::unique_ptr<IPlot> indicatorPlot);

    double lowerRange();
    double upperRange();
    int size();

private:
    // axisRect ref
    QCPAxisRect& axisRect_;

    // graph
    QCPBars* volumeBars_;

    // data container
    QSharedPointer<QCPBarsDataContainer> dataContainer_;

    // active indicators
    std::unordered_map<IndicatorType, std::list<std::unique_ptr<IPlot>>> activeIndicatorPlots_;

    int size_;
};

#endif // VOLUMEPLOT_H
