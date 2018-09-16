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
        replotTimer.start(1000);
    }

private slots:
    void replot()
    {
        static size_t lastPlottedIndex = 0;
        static int numTimesCalled = 0;
        const size_t numPoints = 1000;

        std::cout << "numtimescalled " << ++numTimesCalled << "\n";

        // only plot numPoints at most each time
        for(size_t i = 0; lastPlottedIndex < plotData->ticks->size() && i < numPoints; lastPlottedIndex++, ++i)
        {
            ui->plot->graph()->addData(lastPlottedIndex, (*plotData->ticks)[lastPlottedIndex].price);
        }


        std::cout << "plotting " << "\n";
        ui->plot->replot();
        ui->plot->rescaleAxes();
        Sleep(2000);
    }

private:
    Ui::TheTradingMachineApplication *ui;

    QTimer replotTimer;
};

#endif // THETRADINGMACHINEAPPLICATION_H
