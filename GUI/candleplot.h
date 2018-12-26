#ifndef CANDLEPLOT_H
#define CANDLEPLOT_H

#include "qcustomplot.h"
#include "indicatorgraph.h"
#include "baseplot.h"
#include <unordered_map>
#include <ctime>

class CandlePlot : public BasePlot
{
    Q_OBJECT
public:
    CandlePlot(QCustomPlot& parentPlot);
    ~CandlePlot() override;
    void updatePlotAdd(const Candlestick &candle) override;
    void updatePlotReplace(const Candlestick &candle) override;
    void pastCandlesPlotUpdate(std::shared_ptr<IIndicatorGraph> iplot) override;
    void rescalePlot() override;

    double lowerRange();
    double upperRange();
    int size();

private:
    void indicatorSelectionMenu(QPoint pos) override;

    template<typename IndicatorType, typename... Args>
    void indicatorLaunch(OhlcType valueType, IndicatorDisplayType displayType, Args... args);

    // graph
    QCPFinancial* candleBars_;

    int size_;

private slots:
    void xAxisChanged(QCPRange range) override;

};
#endif // CANDLEPLOT_H
