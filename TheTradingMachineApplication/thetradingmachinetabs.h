#ifndef THETRADINGMACHINETABS_H
#define THETRADINGMACHINETABS_H

#include <QWidget>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QGridLayout>
#include <QTimer>
#include "qcustomplot.h"
#include "SupportBreakShort/SupportBreakShortPlotData.h"

// this is a tab set up for the tab pages in the trading machine
class TheTradingMachineTabs : public QWidget
{
public:
    TheTradingMachineTabs(QWidget* parent = nullptr);
    ~TheTradingMachineTabs();

    // deleted functions because there is no need to duplicate a tab that looks like another
    TheTradingMachineTabs(const TheTradingMachineTabs& other) = delete;
    TheTradingMachineTabs(const TheTradingMachineTabs&& other) = delete;
    TheTradingMachineTabs& operator=(const TheTradingMachineTabs& other) = delete;

    QCustomPlot* plot(void);
    void setPlotData(int instHandle, const PlotData* plotdata);
    void run();
    int getHandle(void);

private:
    QGridLayout *gridLayout_;
    QScrollBar *horizontalScrollBar_;
    QScrollBar *verticalScrollBar_;
    QCustomPlot *plot_;
    int algorithmHandle_;

    // all tabs share a single qtimer. all tab replot slots are connected to this timer's signal
    static QTimer replotTimer_;
    const PlotData* plotData_;
    std::vector<double>::size_type lastPlotDataIndex_;

private slots:
    void updatePlot();
};

#endif // THETRADINGMACHINETABS_H