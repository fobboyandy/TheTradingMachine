#ifndef CANDLEVOLUMEPLOT_H
#define CANDLEVOLUMEPLOT_H

#include "qcustomplot.h"
#include "iplot.h"

// class for controlling and updating candle and volume
class CandleVolumePlot : public IPlot
{
public:
    CandleVolumePlot(QCPAxisRect& candleAxisRect, QCPAxisRect& volumeAxisRect);
    ~CandleVolumePlot() override;
    void updatePlotNewCandle(const time_t candleTime, const Bar &candle) override;
    void updatePlotReplaceCandle(const time_t candleTime, const Bar &candle) override;
    void rescaleValueAxisAutofit() override;

    double lowerRange();
    double upperRange();

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

    int size;
};

#endif // CANDLEVOLUMEPLOT_H
