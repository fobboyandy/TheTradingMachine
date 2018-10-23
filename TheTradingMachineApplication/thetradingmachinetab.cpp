#include "thetradingmachinetab.h"
#include "CandleMaker.h"
#include "playdialog.h"
#include <iostream>

TheTradingMachineTab::TheTradingMachineTab(const AlgorithmApi& api, std::shared_ptr<IBInterfaceClient> client, QWidget* parent) :
    QWidget(parent),
    gridLayout_(nullptr),
    plot_(nullptr),
    replotTimer_(new QTimer(this)),
    api_(api),
    client_(client),
    plotData_(nullptr),
    candleSticksAxisRect_(nullptr),
    candleSticksGraph_(nullptr),
    timeFrame_(60),
    candleMaker_(timeFrame_),
    lastPlotDataIndex_(0),
    volumeAxisRect_(nullptr),
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
    legendSetup();
    spacingSetup();

    // interconnect x axis ranges of main and bottom axis rects:
    connect(candleSticksAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), volumeAxisRect_->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), candleSticksAxisRect_->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));

    // prompt user for the input method. real time or historical ticks
    // std::wstring fpTest("..\\outputfiles\\Jul 19AMD.tickdat");
    PlayDialog loadInput(this);
    loadInput.exec();
    auto input = loadInput.getInput();
    name_ = formatTabName(input);

    if(name_.size() > 0)
    {

        //if real time, check for ib connection
        // instantiate the algorithm for this ticker
        algorithmHandle_ = api_.playAlgorithm(input.toStdString(), &plotData_, client_);
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
    candleSticksAxisRect_ = new QCPAxisRect(plot_);
    candleSticksAxisRect_->setRangeDrag(Qt::Horizontal);
    candleSticksAxisRect_->setRangeZoom(Qt::Horizontal);
    candleSticksGraph_ = new QCPFinancial(candleSticksAxisRect_->axis(QCPAxis::atBottom), candleSticksAxisRect_->axis(QCPAxis::atLeft));
    candleSticksGraph_->setWidthType(QCPFinancial::WidthType::wtPlotCoords);
    candleSticksGraph_->setWidth(timeFrame_ - 2);

    //create the time axis here since we don't need to use the handle later
    auto xTimeAxis = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
    xTimeAxis->setDateTimeFormat("hh:mm:ss");
    candleSticksAxisRect_->axis(QCPAxis::atBottom)->setTicker(xTimeAxis);

    candleBarsDataContainer_ = QSharedPointer<QCPFinancialDataContainer>(new QCPFinancialDataContainer);
    candleSticksGraph_->setData(candleBarsDataContainer_);
    plot_->plotLayout()->addElement(0, 0, candleSticksAxisRect_);

    progressWindow_ = new QCPLayoutInset();
    auto title = new QCPTextElement(plot_);
    title->setText("Title");
    progressWindow_->addElement(title, Qt::AlignTop | Qt::AlignHCenter);
    candleSticksAxisRect_->insetLayout()->addElement(progressWindow_, Qt::AlignTop | Qt::AlignHCenter);

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

    // create two bar plottables, for positive (green) and negative (red) volume bars:
    volumeBarsGraph_ = new QCPBars(volumeAxisRect_->axis(QCPAxis::atBottom), volumeAxisRect_->axis(QCPAxis::atLeft));
    // set the width of each bar to match the candle sticks
    volumeBarsGraph_->setWidth(candleSticksGraph_->width());
    volumeBarsGraph_->setWidthType(QCPBars::WidthType::wtPlotCoords);
    volumeBarsGraph_->setWidth(timeFrame_ - 2);

    volumeBarsGraph_->setPen(Qt::NoPen);
    volumeBarsGraph_->setBrush(QColor(30, 144, 255));
    volumeBarsDataContainer_ = QSharedPointer<QCPBarsDataContainer>(new QCPBarsDataContainer);
    volumeBarsGraph_->setData(volumeBarsDataContainer_);
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
    candleSticksAxisRect_->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    volumeAxisRect_->setMarginGroup(QCP::msLeft|QCP::msRight, group);
}

void TheTradingMachineTab::legendSetup()
{
    candleGraphLegend_ = new QCPLegend();
    candleSticksAxisRect_->insetLayout()->addElement(candleGraphLegend_, Qt::AlignTop | Qt::AlignRight);
    plot_->setAutoAddPlottableToLegend(false);
    candleGraphLegend_->addItem(new QCPPlottableLegendItem(candleGraphLegend_, candleSticksGraph_));
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

void TheTradingMachineTab::updatePlot(void)
{
    std::unique_lock<std::mutex> lock(plotData_->plotDataMtx, std::defer_lock);
    // if gui was running on a faster thread for some reason than the algorithm
    // then simply let the gui do something else while the algorithm is operating
    // on the plotData buffer. neither side has to block
    if(lock.try_lock())
    {
        const size_t plotDataSz = plotData_->ticks.size();
        plotActive_ = !plotData_->finished;
        // according to stl, "Concurrently accessing or modifying different elements is safe."
        // as long as other thread is always pushing to the end and we are accessing the middle,
        // the rule is satisfied
        lock.unlock();

        for(; lastPlotDataIndex_ < plotDataSz; ++lastPlotDataIndex_)
        {
            // candleTime holds the time of the most recent candle
            bool isNewCandle = candleMaker_.updateCandle(plotData_->ticks[lastPlotDataIndex_], currentCandle_);
            // getUpdatedCandleTime will return the updated time to the nearest timeFrame
            auto currentCandleTime = candleMaker_.getUpdatedCandleTime();
			
            if(isNewCandle)
            {
                // add a new bar to the back
                candleBarsDataContainer_->add(QCPFinancialData(currentCandleTime, currentCandle_.open, currentCandle_.high, currentCandle_.low, currentCandle_.close));
                volumeBarsDataContainer_->add(QCPBarsData(currentCandleTime, currentCandle_.volume));
            }
            //keep the most recent added candle up to date
            else if(candleBarsDataContainer_->size() > 0)
            {
                candleBarsDataContainer_->set(candleBarsDataContainer_->size() - 1, QCPFinancialData(currentCandleTime , currentCandle_.open, currentCandle_.high, currentCandle_.low, currentCandle_.close));
                volumeBarsDataContainer_->set(volumeBarsDataContainer_->size() - 1, QCPBarsData(currentCandleTime, currentCandle_.volume));
            }
        }

        if(autoScale_)
        {
            candleSticksGraph_->rescaleAxes();
            volumeBarsGraph_->rescaleAxes();
        }

        if(!plotActive_)
        {
            replotTimer_->stop();
        }

        //replot should always be happening to update the drawing
        plot_->replot();
    }
}

void TheTradingMachineTab::xAxisChanged(QCPRange range)
{
    if(floor(range.lower) <= candleSticksGraph_->data()->at(0)->mainKey() &&
        ceil(range.upper) >= candleSticksGraph_->data()->at(candleSticksGraph_->data()->size() - 1)->mainKey())
    {
        autoScale_ = true;
    }
    else
    {
        autoScale_ = false;
        candleSticksGraph_->rescaleValueAxis(false, true);
        volumeBarsGraph_->rescaleValueAxis(false, true);
    }
}

