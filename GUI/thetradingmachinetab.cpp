#include "thetradingmachinetab.h"
#include "CandleMaker.h"
#include "playdialog.h"
#include <iostream>
#include "../Indicators/Indicators/SimpleMovingAverage.h"

TheTradingMachineTab::TheTradingMachineTab(const AlgorithmApi& api, std::shared_ptr<InteractiveBrokersClient> client, QWidget* parent) :
    QWidget(parent),
    gridLayout_(nullptr),
    plot_(nullptr),
    replotTimer_(new QTimer(this)),
    api_(api),
    client_(client),
    plotData_(nullptr),
    timeFrame_(60),
    candleMaker_(timeFrame_),
    lastPlotDataIndex_(0),
    autoScale_(true),
    plotActive_(false),
    valid_(false)
{
    this->setObjectName(QStringLiteral("TheTradingMachineTab"));
    gridLayout_ = new QGridLayout(this);
    gridLayout_->setObjectName(QStringLiteral("gridLayout"));
    plot_ = new QCustomPlot(this);
    plot_->plotLayout()->clear(); //remove all layouts so we can start from scratch
    plot_->setObjectName(QStringLiteral("plot"));
    plot_->setInteraction(QCP::iRangeDrag);
    plot_->setInteraction(QCP::iRangeZoom);
    gridLayout_->addWidget(plot_, 0, 0, 1, 1);

    candleGraphSetup();
    volumeGraphSetup();
    candleVolume_ = std::make_unique<CandleVolumePlot>(*candleAxisRect_, *volumeAxisRect_);
    legendSetup();
    spacingSetup();

    // interconnect x axis ranges of main and bottom axis rects:
    connect(candleAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), volumeAxisRect_->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), candleAxisRect_->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));

    // prompt user for the input method. real time or historical ticks
    PlayDialog loadInput(this);
    loadInput.exec();
    auto input = loadInput.getInput();
    name_ = formatTabName(input);

    if(name_.size() > 0)
    {
        //if real time, check for ib connection
        // instantiate the algorithm for this ticker
        algorithmHandle_ = api_.playAlgorithm(input.toStdString(), &plotData_, client_, false);
        if(algorithmHandle_ != -1)
        {
            // tab should only be valid if play algorithm and getplotdata worked
            valid_ = true;
            connect(replotTimer_, &QTimer::timeout, this, &TheTradingMachineTab::updatePlot);
            replotTimer_->start(50);
        }
    }
}

TheTradingMachineTab::~TheTradingMachineTab()
{
    // we should call stop algorithm even if
    // valid_ is false to let dll do any
    // necessary clean up
    if(!api_.stopAlgorithm(algorithmHandle_))
    {
        qDebug("Unable to stop algorithm!!!");
    }
}

bool TheTradingMachineTab::valid() const
{
    return valid_;
}

QString TheTradingMachineTab::tabName() const
{
    return name_;
}

void TheTradingMachineTab::candleGraphSetup()
{
    //set up Candle Stick Graph
    candleAxisRect_ = new QCPAxisRect(plot_);
    candleAxisRect_->setRangeDrag(Qt::Horizontal);
    candleAxisRect_->setRangeZoom(Qt::Horizontal);
    //create the time axis here since we don't need to use the handle later
    auto xTimeAxis = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
    xTimeAxis->setDateTimeFormat("hh:mm:ss");
    candleAxisRect_->axis(QCPAxis::atBottom)->setTicker(xTimeAxis);
    plot_->plotLayout()->addElement(0, 0, candleAxisRect_);
}

void TheTradingMachineTab::volumeGraphSetup()
{
    //set up Volume graph
    volumeAxisRect_ = new QCPAxisRect(plot_);
    volumeAxisRect_->setRangeDrag(Qt::Horizontal);
    volumeAxisRect_->setRangeZoom(Qt::Horizontal);
    volumeAxisRect_->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
    volumeAxisRect_->axis(QCPAxis::atBottom)->setLayer("axes");
    volumeAxisRect_->axis(QCPAxis::atBottom)->grid()->setLayer("grid");

    //create the time axis here since we don't need to use the handle later
    auto xTimeAxis = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
    xTimeAxis->setDateTimeFormat("hh:mm:ss");
    volumeAxisRect_->axis(QCPAxis::atBottom)->setTicker(xTimeAxis);
    plot_->plotLayout()->addElement(1, 0, volumeAxisRect_);
}

void TheTradingMachineTab::spacingSetup()
{
    // bring bottom and main axis rect closer together:
    plot_->plotLayout()->setRowSpacing(0);
    volumeAxisRect_->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
    volumeAxisRect_->setMargins(QMargins(0, 0, 0, 0));

    // make axis rects' left side line up:
    QCPMarginGroup *group = new QCPMarginGroup(plot_);
    candleAxisRect_->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    volumeAxisRect_->setMarginGroup(QCP::msLeft|QCP::msRight, group);
}

void TheTradingMachineTab::legendSetup()
{
//    candleGraphLegend_ = new QCPLegend();
//    candleSticksAxisRect_->insetLayout()->addElement(candleGraphLegend_, Qt::AlignTop | Qt::AlignRight);
//    plot_->setAutoAddPlottableToLegend(false);
//    candleGraphLegend_->addItem(new QCPPlottableLegendItem(candleGraphLegend_, candleSticksGraph_));
}

QString TheTradingMachineTab::formatTabName(const QString &input)
{
    QString inputFormatted;
    // for recorded files, strip away the directory names by adding
    // characters from the back to the front of the buffer
    auto extensionIndex = input.toStdWString().find(L".tickdat");
    if(extensionIndex != std::wstring::npos)
    {
        for(auto i = static_cast<QString::size_type>(extensionIndex) - 1; i >= 0 && input[i] != '\\' && input[i] != '/'; --i)
        {
            inputFormatted.push_front(input[i]);
        }
    }
    else
    {
        inputFormatted = input;
    }
    return inputFormatted;
}

void TheTradingMachineTab::updatePlotNewCandle(const time_t candleTime, const Bar &candle)
{
    // add new bar to all active indicators' plots
    candleVolume_->updatePlotNewCandle(candleTime, candle);
}

void TheTradingMachineTab::updatePlotReplaceCandle(const time_t candleTime, const Bar &candle)
{
    //update active indicators' plots
    candleVolume_->updatePlotReplaceCandle(candleTime, candle);
}

void TheTradingMachineTab::updatePlot(void)
{
    std::unique_lock<std::mutex> lock(plotData_->plotDataMtx);

    const size_t plotDataSz = plotData_->ticks.size();
    plotActive_ = !plotData_->finished;
    // according to stl, "Concurrently accessing or modifying different elements is safe."
    // as long as other thread is always pushing to the end and we are accessing the middle,
    // the rule is satisfied
    lock.unlock();

    for(; lastPlotDataIndex_ < plotDataSz; ++lastPlotDataIndex_)
    {
        time_t currentCandleTime;
        // candleTime holds the time of the most recent candle
        bool isNewCandle = candleMaker_.updateCandle(plotData_->ticks[lastPlotDataIndex_], currentCandle_, currentCandleTime);
        // getUpdatedCandleTime will return the updated time to the nearest timeFrame

        //update the plot with a new candle.
        if(isNewCandle)
        {
            candleVolume_->updatePlotNewCandle(currentCandleTime, currentCandle_);
        }
        //keep the plot up to date with an updated candle
        else
        {
            candleVolume_->updatePlotReplaceCandle(currentCandleTime, currentCandle_);
        }
    }

    if(autoScale_)
    {
        plot_->rescaleAxes();
    }

    if(!plotActive_)
    {
        replotTimer_->stop();
    }

    //replot should always be happening to update the drawing
    plot_->replot();
}

void TheTradingMachineTab::xAxisChanged(QCPRange range)
{
    if(floor(range.lower) <= candleVolume_->lowerRange() &&
        ceil(range.upper) >= candleVolume_->upperRange())
    {
        autoScale_ = true;
    }
    else
    {
        autoScale_ = false;
        candleVolume_->rescaleValueAxisAutofit();
    }
}

enum class TheTradingMachineTab::IPlotIndex
{
    CANDLE,
    VOLUME,
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

