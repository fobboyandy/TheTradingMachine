#ifndef THETRADINGMACHINEAPPLICATION_H
#define THETRADINGMACHINEAPPLICATION_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "SupportBreakShort/SupportBreakShortPlotData.h"
#include "ui_thetradingmachineapplication.h"
#include <iostream>
#include <Windows.h>

namespace Ui {
class TheTradingMachineApplication;
}

class TheTradingMachineApplication : public QMainWindow
{
    Q_OBJECT

public:
    explicit TheTradingMachineApplication(QWidget *parent = nullptr);
    ~TheTradingMachineApplication();
    SupportBreakShortPlotData::PlotData* plotData {nullptr};
    void initializePlotData(SupportBreakShortPlotData::PlotData* plotDataIn)
    {
        plotData = plotDataIn;
        connect(&this->replotTimer, &QTimer::timeout, this, &TheTradingMachineApplication::replot);
        replotTimer.start(); //fastest possible refresh rate
    }

private slots:
    void replot()
    {
        static size_t lastPlottedIndex = 0;

        // only plot numPoints at most each time
        for(; lastPlottedIndex < plotData->ticks->size() ; ++lastPlottedIndex)
        {
            ui->plot->graph()->addData(lastPlottedIndex, (*plotData->ticks)[lastPlottedIndex].price);
        }

        ui->plot->replot();
        ui->plot->rescaleAxes();
    }

private:
    Ui::TheTradingMachineApplication *ui;

    QTimer replotTimer;
};

#endif // THETRADINGMACHINEAPPLICATION_H
