#ifndef VOLUMEPLOT_H
#define VOLUMEPLOT_H

#include "qcustomplot.h"
#include "../InteractiveBrokersApi/bar.h"
#include "indicatorplot.h"
#include "baseplot.h"
#include <unordered_map>
#include <ctime>

class VolumePlot : public BasePlot
{
    Q_OBJECT
public:
    VolumePlot(QCustomPlot& t_parentPlot);
    ~VolumePlot() override;
    void updatePlotAdd(const time_t candleTime, const Bar &candle) override;
    void updatePlotReplace(const time_t candleTime, const Bar &candle) override;
    void rescalePlot() override;
    void pastCandlesPlotUpdate(std::shared_ptr<IIndicatorPlot> iplot) override;
    void addIndicator(IndicatorType indicatorType, std::unique_ptr<IIndicatorPlot> indicatorPlot);

    double lowerRange();
    double upperRange();
    int size();

private slots:
    void xAxisChanged(QCPRange range) override;

private:
    // graph
    QCPBars* volumeBars_;

    // active indicators
    std::unordered_map<IndicatorType, std::list<std::unique_ptr<IIndicatorPlot>>> activeIndicatorPlots_;

    int size_;
};

#endif // VOLUMEPLOT_H
