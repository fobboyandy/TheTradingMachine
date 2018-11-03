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
#include "candlevolumeplot.h"

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
    // keep CandleVolumePlot_ separately since we need to access the
    // it to maintain the layout of the rects
    std::unique_ptr<CandleVolumePlot> candleVolumePlot_;

    // candle data
    Bar currentCandle_;
    int timeFrame_;
    CandleMaker candleMaker_;
    std::vector<double>::size_type lastPlotDataIndex_;

    // enumeration for predefined mapping for various indicators
    enum class IPlotIndex;

    // all activated plots
    std::unordered_map<IPlotIndex, std::list<std::unique_ptr<IPlot>>> activePlots_;

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

// define IPlotIndex
enum class TheTradingMachineTab::IPlotIndex
{
    CANDLEVOLUME,
    ABS,
    ACOS,
    AD,
    ADD,
    ADOSC,
    ADX,
    ADXR,
    AO,
    APO,
    AROON,
    AROONOSC,
    ASIN,
    ATAN,
    ATR,
    AVGPRICE,
    BBANDS,
    BOP,
    CCI,
    CEIL,
    CMO,
    COS,
    COSH,
    CROSSANY,
    CROSSOVER,
    CVI,
    DECAY,
    DEMA,
    DI,
    DIV,
    DM,
    DPO,
    DX,
    EDECAY,
    EMA,
    EMV,
    EXP,
    FISHER,
    FLOOR,
    FOSC,
    HMA,
    KAMA,
    KVO,
    LAG,
    LINREG,
    LINREGINTERCEPT,
    LINREGSLOPE,
    LN,
    LOG10,
    MACD,
    MARKETFI,
    MASS,
    MAX,
    MD,
    MEDPRICE,
    MFI,
    MIN,
    MOM,
    MSW,
    MUL,
    NATR,
    NVI,
    OBV,
    PPO,
    PSAR,
    PVI,
    QSTICK,
    ROC,
    ROCR,
    ROUND,
    RSI,
    SIN,
    SINH,
    SMA,
    SQRT,
    STDDEV,
    STDERR,
    STOCH,
    STOCHRSI,
    SUB,
    SUM,
    TAN,
    TANH,
    TEMA,
    TODEG,
    TORAD,
    TR,
    TRIMA,
    TRIX,
    TRUNC,
    TSF,
    TYPPRICE,
    ULTOSC,
    VAR,
    VHF,
    VIDYA,
    VOLATILITY,
    VOSC,
    VWMA,
    WAD,
    WCPRICE,
    WILDERS,
    WILLR,
    WMA,
    ZLEMA
};

#endif // THETRADINGMACHINETAB_H
