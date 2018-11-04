#ifndef THETRADINGMACHINETAB_H
#define THETRADINGMACHINETAB_H

#include <QWidget>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QGridLayout>
#include <QTimer>
#include <memory>
#include <thread>
#include <unordered_map>
#include <list>
#include "qcustomplot.h"
#include "CandleMaker.h"
#include "../InteractiveBrokersClient/InteractiveBrokersClient/InteractiveBrokersClient.h"
#include "../BaseAlgorithm/BaseAlgorithm/PlotData.h"
#include "iplot.h"
#include "candleplot.h"
#include "volumeplot.h"

// this is a tab set up for the tab pages in the trading machine
class TheTradingMachineTab : public QWidget
{
    Q_OBJECT
public:
    struct AlgorithmApi
    {
        using PlayAlgorithmFnPtr = int (*)(std::string, std::shared_ptr<PlotData>*, std::shared_ptr<InteractiveBrokersClient>, bool);
        using StopAlgorithmFnPtr = bool (*)(int);

        std::function<int(std::string, std::shared_ptr<PlotData>*, std::shared_ptr<InteractiveBrokersClient>, bool)> playAlgorithm;
        std::function<bool(int)> stopAlgorithm;
    };

    TheTradingMachineTab(const AlgorithmApi& api, std::shared_ptr<InteractiveBrokersClient> client, QWidget* parent = nullptr);
    ~TheTradingMachineTab();

    // deleted functions because there is no need to duplicate a tab that looks like another
    TheTradingMachineTab(const TheTradingMachineTab& other) = delete;
    TheTradingMachineTab(const TheTradingMachineTab&& other) = delete;
    TheTradingMachineTab& operator=(const TheTradingMachineTab& other) = delete;

    QString tabName() const;
    bool valid() const;

private:
    // plot items
    QGridLayout *gridLayout_;
    QCustomPlot *plot_;
    QTimer* replotTimer_;
    QString name_;
    QMenu *plotRightClickMenu;

    //algorithm api
    AlgorithmApi api_;
    int algorithmHandle_;
    std::shared_ptr<InteractiveBrokersClient> client_;
    std::shared_ptr<PlotData> plotData_;

    // upper and lower axis rect. upper for candle lower for volume
    QCPAxisRect* candleAxisRect_;
    QCPAxisRect* volumeAxisRect_;
    std::unique_ptr<CandlePlot> candlePlot_;
    std::unique_ptr<VolumePlot> volumePlot_;

    // candle data
    Bar currentCandle_;
    int timeFrame_;
    CandleMaker candleMaker_;
    std::vector<double>::size_type lastPlotDataIndex_;

    // all activated plots
    std::unordered_map<IPlot::IPlotIndex, std::list<std::unique_ptr<IPlot>>> activeCandlePlots_;
    std::unordered_map<IPlot::IPlotIndex, std::list<std::unique_ptr<IPlot>>> activeVolumePlots_;

    //plot scale control
    bool autoScale_;
    bool plotActive_;
    bool valid_;

private:
    void layoutSetup();
    void plotRightClickMenuSetup();

    QString formatTabName(const QString& input);

private:
    void updatePlotNewCandle(const time_t candleTime, const Bar &candle);
    void updatePlotReplaceCandle(const time_t candleTime, const Bar &candle);

private slots:
    void updatePlot(void);
    void xAxisChanged(QCPRange range);
    void contextMenuRequest(QPoint pos);

};

#endif // THETRADINGMACHINETAB_H
