#ifndef THETRADINGMACHINETABS_H
#define THETRADINGMACHINETABS_H

#include <QWidget>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QGridLayout>
#include <QTimer>
#include <memory>
#include <thread>
#include "qcustomplot.h"
#include "TheTradingMachine.h"

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

    void playPlotData(int instHandle, std::shared_ptr<PlotData> plotdata);
    int getHandle(void);

private:
    QGridLayout *gridLayout_;
    QScrollBar *horizontalScrollBar_;
    QScrollBar *verticalScrollBar_;
    QCustomPlot *plot_;
    int algorithmHandle_;

    std::shared_ptr<PlotData> plotData_;
    std::vector<double>::size_type lastPlotDataIndex_;

    std::unique_ptr<std::thread> plotThread_;
    std::atomic<bool> runPlotThread_;

    QTimer replotTimer_;

private slots:
    void updatePlot(void);
};

#endif // THETRADINGMACHINETABS_H
