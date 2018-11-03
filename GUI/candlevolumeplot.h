#ifndef CANDLEVOLUMEPLOT_H
#define CANDLEVOLUMEPLOT_H

#include "qcustomplot.h"
#include "iplot.h"

// class for controlling and updating candle and volume
class CandleVolumePlot
{
public:
    CandleVolumePlot(QCPAxisRect& candleAxisRect, QCPAxisRect& volumeAxisRect);
    ~CandleVolumePlot() ;
    void updatePlotNewCandle(const time_t candleTime, const Bar &candle);
    void updatePlotReplaceCandle(const time_t candleTime, const Bar &candle);
    void rescaleValueAxisAutofit();

    double lowerRange();
    double upperRange();
    int size();

private:
    // axisRect ref
    QCPAxisRect& candleAxisRect_;
    QCPAxisRect& volumeAxisRect_;

    // graph
    QCPFinancial* candleBars_;
    QCPBars* volumeBars_;

    // data containers
    QSharedPointer<QCPFinancialDataContainer> candleBarsDataContainer_;
    QSharedPointer<QCPBarsDataContainer> volumeBarsDataContainer_;

    int size_;
};

#endif // CANDLEVOLUMEPLOT_H
