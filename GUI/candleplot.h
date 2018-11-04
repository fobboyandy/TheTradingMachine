#ifndef CANDLEPLOT_H
#define CANDLEPLOT_H

#include "qcustomplot.h"
#include "../InteractiveBrokersApi/bar.h"
#include "iplot.h"
#include <unordered_map>
#include <ctime>

class CandlePlot
{
public:
    CandlePlot(QCPAxisRect& axisRect);
    ~CandlePlot() ;
    void updatePlotAdd(const time_t candleTime, const Bar &candle);
    void updatePlotReplace(const time_t candleTime, const Bar &candle);
    void rescaleValueAxisAutofit();
    void addIndicator(IPlot::IndicatorType indicatorType, std::unique_ptr<IPlot> indicatorPlot);

    double lowerRange();
    double upperRange();
    int size();

private:
    // axisRect ref
    QCPAxisRect& axisRect_;

    // graph
    QCPFinancial* candleBars_;

    // data container
    QSharedPointer<QCPFinancialDataContainer> dataContainer_;

    // active indicators
    std::unordered_map<IPlot::IndicatorType, std::list<std::unique_ptr<IPlot>>> activeIndicatorPlots_;

    int size_;
};
#endif // CANDLEPLOT_H
