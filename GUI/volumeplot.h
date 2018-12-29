#ifndef VOLUMEPLOT_H
#define VOLUMEPLOT_H

#include "qcustomplot.h"
#include "indicatorgraph.h"
#include "baseplot.h"
#include <unordered_map>
#include <ctime>

class VolumePlot : public BasePlot
{
    Q_OBJECT
public:
    VolumePlot(QCustomPlot& t_parentPlot);
    ~VolumePlot() override;
    void updatePlotAdd(const Candlestick &candle) override;
    void updatePlotReplace(const Candlestick &candle) override;
    void rescalePlot() override;
    void pastCandlesPlotUpdate(std::shared_ptr<IIndicatorGraph> iplot) override;
    void addIndicator(IndicatorType indicatorType, std::unique_ptr<IIndicatorGraph> indicatorPlot);

    double lowerRange();
    double upperRange();
    int size();

private slots:
    void xAxisChanged(QCPRange range) override;

private:
    // graph
    QCPBars* volumeBars_;
    int size_;
    void indicatorSelectionMenu(QPoint pos) override;

    template<typename IndicatorType, typename... Args>
    void indicatorLaunch(OhlcType valueType, IndicatorDisplayType displayType, Args... args);

};

#endif // VOLUMEPLOT_H
