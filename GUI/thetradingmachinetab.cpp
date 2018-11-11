#include "thetradingmachinetab.h"
#include "CandleMaker.h"
#include "playdialog.h"
#include <iostream>
#include "../Indicators/Indicators/SimpleMovingAverage.h"
#include "indicatorplot.h"

TheTradingMachineTab::TheTradingMachineTab(const AlgorithmApi& api, std::shared_ptr<InteractiveBrokersClient> client, QWidget* parent) :
    QWidget(parent),
    replotTimer_(new QTimer(this)),
    api_(api),
    client_(client),
    timeFrame_(60),
    candleMaker_(timeFrame_),
    lastPlotDataIndex_(0),
    autoScale_(true),
    plotActive_(false),
    valid_(false)
{
    // this sets up the axis rect necessary for our plots
    layoutSetup();
    // create basic plots
    candlePlot_ = std::make_unique<CandlePlot>(*plot_, *candleAxisRect_);
    volumePlot_ = std::make_unique<VolumePlot>(*volumeAxisRect_);

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

void TheTradingMachineTab::layoutSetup()
{
    // allocate memory and set up each item in the plot
    this->setObjectName(QStringLiteral("TheTradingMachineTab"));
    gridLayout_ = new QGridLayout(this);
    gridLayout_->setObjectName(QStringLiteral("gridLayout"));
    plot_ = new QCustomPlot(this);
    plot_->plotLayout()->clear(); //remove all layouts so we can start from scratch
    plot_->setObjectName(QStringLiteral("plot"));
    plot_->setInteraction(QCP::iRangeDrag);
    plot_->setInteraction(QCP::iRangeZoom);
    plot_->setInteraction(QCP::Interaction::iSelectPlottables);
    gridLayout_->addWidget(plot_, 0, 0, 1, 1);

    // create a time axis. we don't plan on modifying
    // the time axis so we dont need to keep the handle.
    // once axis rect has ownership, it will do the clean up
    auto xTimeAxis = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
    xTimeAxis->setDateTimeFormat("hh:mm:ss");

    // set up candle rect
    candleAxisRect_ = new QCPAxisRect(plot_);
    candleAxisRect_->setRangeDrag(Qt::Horizontal);
    candleAxisRect_->setRangeZoom(Qt::Horizontal);
    candleAxisRect_->axis(QCPAxis::atBottom)->setTicker(xTimeAxis);
    plot_->plotLayout()->addElement(0, 0, candleAxisRect_);

    // set up volume rect
    volumeAxisRect_ = new QCPAxisRect(plot_);
    volumeAxisRect_->setRangeDrag(Qt::Horizontal);
    volumeAxisRect_->setRangeZoom(Qt::Horizontal);
    volumeAxisRect_->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
    volumeAxisRect_->axis(QCPAxis::atBottom)->setLayer("axes");
    volumeAxisRect_->axis(QCPAxis::atBottom)->grid()->setLayer("grid");
    volumeAxisRect_->axis(QCPAxis::atBottom)->setTicker(xTimeAxis);
    plot_->plotLayout()->addElement(1, 0, volumeAxisRect_);

    // set up the spacing
    plot_->plotLayout()->setRowSpacing(0);
    volumeAxisRect_->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
    volumeAxisRect_->setMargins(QMargins(0, 0, 0, 0));

    // make axis rects' left side line up:
    QCPMarginGroup *group = new QCPMarginGroup(plot_);
    candleAxisRect_->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    volumeAxisRect_->setMarginGroup(QCP::msLeft|QCP::msRight, group);

    // make upper and lower rects scroll together
    // all active plots range will be notified of the change since they are registered
    // to volume and candle rect axes
    connect(candleAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), volumeAxisRect_->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), candleAxisRect_->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
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
    candlePlot_->updatePlotAdd(candleTime, candle);
    volumePlot_->updatePlotAdd(candleTime, candle);
}

void TheTradingMachineTab::updatePlotReplaceCandle(const time_t candleTime, const Bar &candle)
{
    candlePlot_->updatePlotReplace(candleTime, candle);
    volumePlot_->updatePlotReplace(candleTime, candle);
}

void TheTradingMachineTab::updatePlot(void)
{
    std::unique_lock<std::mutex> lock(plotData_->plotDataMtx);

    const size_t plotDataSz = plotData_->ticks.size();
    // according to stl, "Concurrently accessing or modifying different elements is safe."
    // as long as other thread is always pushing to the end and we are accessing the middle,
    // the rule is satisfied
    lock.unlock();

    for(; lastPlotDataIndex_ < plotDataSz; ++lastPlotDataIndex_)
    {
        Bar candle;
        // candleTime holds the time of the most recent candle
        bool isNewCandle = candleMaker_.updateCandle(plotData_->ticks[lastPlotDataIndex_], candle);
        auto candleTime = candleMaker_.getUpdatedCandleTime();

        //update the plot with a new candle.
        if(isNewCandle)
        {
            updatePlotNewCandle(candleTime, candle);
        }
        //keep the plot up to date with an updated candle
        else
        {
            updatePlotReplaceCandle(candleTime, candle);
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
    if(floor(range.lower) <= candlePlot_->lowerRange() &&
        ceil(range.upper) >= candlePlot_->upperRange())
    {
        autoScale_ = true;
    }
    else
    {
        autoScale_ = false;
        //rescale everything
        candlePlot_->rescaleValueAxisAutofit();
    }
}
