#ifndef CANDLEPLOT_H
#define CANDLEPLOT_H

#include "qcustomplot.h"
#include "../InteractiveBrokersApi/bar.h"
#include "iplot.h"
#include <unordered_map>
#include <ctime>

class CandlePlot : public QObject
{
    Q_OBJECT
public:
    CandlePlot(QCustomPlot& parentPlot, QCPAxisRect& axisRect);
    ~CandlePlot() ;
    void updatePlotAdd(const time_t candleTime, const Bar &candle);
    void updatePlotReplace(const time_t candleTime, const Bar &candle);
    void pastCandlesPlotUpdate(std::shared_ptr<IPlot> iplot);

    void rescaleValueAxisAutofit();
    void indicatorSelectionMenu(QPoint pos);
    void removeIndicatorMenu(QPoint pos, QList<QCPAbstractPlottable*> plottables);

    double lowerRange();
    double upperRange();
    int size();

    template<typename IndicatorType, typename... Args>
    void indicatorLaunch(OhlcType valueType, IndicatorDisplayType displayType, Args... args);

private:
    // keep a reference to the parent plot to remove an indicator
    QCustomPlot& parentPlot_;

    // axisRect ref
    QCPAxisRect& axisRect_;

    // graph
    QCPFinancial* candleBars_;

    // data container
    QSharedPointer<QCPFinancialDataContainer> dataContainer_;

    // to create a new indicator entry, we first instantiate an iplot and get the
    // associated plottables. we create an entry and map the plottable to the new iplot.
    // when we select a graph in qcp for removal, we first get back a qcpabstractplottable
    // pointer from qcustomplot we can use this pointer to map to the iplot that it
    // corresponds to and remove the entry. refer to showMenu
    // we use a shared_ptr for multiple instances to the same iplot
    std::unordered_map<QCPAbstractPlottable*, std::shared_ptr<IPlot>> activeIndicatorPlots_;

    int size_;

private slots:
    void menuShowSlot(QPoint pos);
    void plotSelectslot(bool selected);

};
#endif // CANDLEPLOT_H
