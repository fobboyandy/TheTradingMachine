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
#include "CandleMaker.h"

// this is a tab set up for the tab pages in the trading machine
class TheTradingMachineTab : public QWidget
{
    Q_OBJECT
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
    QTimer* replotTimer_;

    //algorithm api
    AlgorithmApi api_;
    int algorithmHandle_;
    IBInterfaceClient* client_;
    std::shared_ptr<PlotData> plotData_;

    //inside candle graph rect
    QCPAxisRect* candleSticksAxisRect_;
    QCPFinancial* candleSticksGraph_;
    QCPLegend* candleGraphLegend_;
    QCPTextElement* candleGraphTitle;
    QSharedPointer<QCPFinancialDataContainer> candleBarsDataContainer_;
    Bar currentCandle_;
    CandleMaker candleMaker_;
    std::vector<double>::size_type lastPlotDataIndex_;
    QCPLayoutInset* progressWindow_;

    //inside volume graph rect
    QCPAxisRect* volumeAxisRect_;
    QCPBars* volumeBarsGraph_;
    QSharedPointer<QCPBarsDataContainer> volumeBarsDataContainer_;

    //plot scale control
    bool autoScale_;
    bool plotActive_;

private:
    void candleGraphSetup(void);
    void volumeGraphSetup(void);
    void spacingSetup(void);
    void legendSetup(void);

private slots:
    void updatePlot(void);
    void xAxisChanged(QCPRange range);
};

#endif // THETRADINGMACHINETAB_H
