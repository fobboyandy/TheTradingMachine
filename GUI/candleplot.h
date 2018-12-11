#ifndef CANDLEPLOT_H
#define CANDLEPLOT_H

#include "qcustomplot.h"
#include "indicatorplot.h"
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
    void pastCandlesPlotUpdate(std::shared_ptr<IIndicatorPlot> iplot) override;
    void rescalePlot() override;

    void indicatorSelectionMenu(QPoint pos);
    void removeIndicatorMenu(QPoint pos, QList<QCPAbstractPlottable*> plottables);

    double lowerRange();
    double upperRange();
    int size();

    template<typename IndicatorType, typename... Args>
    void indicatorLaunch(OhlcType valueType, IndicatorDisplayType displayType, Args... args);

private:
    // graph
    QCPFinancial* candleBars_;

    // to create a new indicator entry, we first instantiate an iplot and get the
    // associated plottables. we create an entry and map the plottable to the new iplot.
    // when we select a graph in qcp for removal, we first get back a qcpabstractplottable
    // pointer from qcustomplot we can use this pointer to map to the iplot that it
    // corresponds to and remove the entry. refer to showMenu
    // we use a shared_ptr for multiple instances to the same iplot
    std::unordered_map<QCPAbstractPlottable*, std::shared_ptr<IIndicatorPlot>> activeIndicatorPlots_;

    int size_;

private slots:
    void menuShowSlot(QPoint pos);
    void plotSelectSlot(bool selected);
    void xAxisChanged(QCPRange range) override;

};
#endif // CANDLEPLOT_H
