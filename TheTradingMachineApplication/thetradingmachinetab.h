#ifndef THETRADINGMACHINETAB_H
#define THETRADINGMACHINETAB_H

#include <QWidget>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QGridLayout>
#include <QTimer>
#include <memory>
#include <thread>
#include "qcustomplot.h"
#include "TheTradingMachine.h"

// this is a tab set up for the tab pages in the trading machine
class TheTradingMachineTab : public QWidget
{
public:
    struct AlgorithmApi
    {
        std::function<int(std::string, IBInterfaceClient*)> playAlgorithm;
        std::function<bool(int, std::shared_ptr<PlotData>**)> getPlotData;
        std::function<bool(int)> stopAlgorithm;
        std::function<bool(void)> unloadAlgorithm;
    };

    TheTradingMachineTab(const AlgorithmApi& api, IBInterfaceClient* client, QWidget* parent = nullptr);
    ~TheTradingMachineTab();

    // deleted functions because there is no need to duplicate a tab that looks like another
    TheTradingMachineTab(const TheTradingMachineTab& other) = delete;
    TheTradingMachineTab(const TheTradingMachineTab&& other) = delete;
    TheTradingMachineTab& operator=(const TheTradingMachineTab& other) = delete;

private:
    QGridLayout *gridLayout_;
    QCustomPlot *plot_;
    int algorithmHandle_;

    std::shared_ptr<PlotData> plotData_;
    std::vector<double>::size_type lastPlotDataIndex_;
    QTimer* replotTimer_;

    AlgorithmApi api_;
    IBInterfaceClient* client_;

private slots:
    void updatePlot(void);
};

#endif // THETRADINGMACHINETAB_H
