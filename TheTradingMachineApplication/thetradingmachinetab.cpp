#include "thetradingmachinetab.h"
#include "CandleMaker.h"

TheTradingMachineTab::TheTradingMachineTab(const AlgorithmApi& api, IBInterfaceClient* client, QWidget* parent) :
    QWidget(parent),
    gridLayout_(nullptr),
    plot_(nullptr),
    replotTimer_(new QTimer(this)),
    plotData_(nullptr),
    candleMaker_(60),
    lastPlotDataIndex_(0),
    api_(api),
    client_(client),
    candleSticksAxisRect_(nullptr),
    candleSticksGraph_(nullptr),
    volumeAxisRect_(nullptr),
    autoScale_(true),
    plotActive_(false)
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
    spacingSetup();

    // interconnect x axis ranges of main and bottom axis rects:
    connect(candleSticksAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), volumeAxisRect_->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), candleSticksAxisRect_->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
    connect(volumeAxisRect_->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));

    //prompt user for the input method. real time or historical ticks
    std::string fpTest("..\\outputfiles\\Jul 24AMD.tickdat");
    //if real time, check for ib connection

    // instantiate the algorithm for this ticker
    algorithmHandle_ = api_.playAlgorithm(fpTest, client_);

    // retrieve the plot data and assign it to the tab
    std::shared_ptr<PlotData>* plotDataOut = nullptr;
    if(api_.getPlotData(algorithmHandle_, &plotDataOut) && plotDataOut != nullptr)
    {
        // plotData_ is a shared pointer and will now have ownership to the shared_ptr
        // created from the dll
        plotData_ = *plotDataOut;
        connect(replotTimer_, &QTimer::timeout, this, &TheTradingMachineTab::updatePlot);
        replotTimer_->start(0);
    }
}

TheTradingMachineTab::~TheTradingMachineTab()
{
    // stop the algorithm dll
    if(!api_.stopAlgorithm(algorithmHandle_))
    {
        qDebug("Unable to stop algorithm!!!");
        assert(false);
    }
}

void TheTradingMachineTab::candleGraphSetup()
{
    //set up Candle Stick Graph
    candleSticksAxisRect_ = new QCPAxisRect(plot_);
    candleSticksAxisRect_->setRangeDrag(Qt::Horizontal);
    candleSticksAxisRect_->setRangeZoom(Qt::Horizontal);
    candleSticksGraph_ = new QCPFinancial(candleSticksAxisRect_->axis(QCPAxis::atBottom), candleSticksAxisRect_->axis(QCPAxis::atLeft));
    candleSticksGraph_->setWidthType(QCPFinancial::WidthType::wtPlotCoords);
    // sentinel element so that we can always replace the previous element instead of adding an if statement
    // which will be evaluated every time
    candleBarsDataContainer_ = QSharedPointer<QCPFinancialDataContainer>(new QCPFinancialDataContainer);
    candleBarsDataContainer_->add(QCPFinancialData(0, 0, 0, 0, 0));
    candleSticksGraph_->setData(candleBarsDataContainer_);
    plot_->plotLayout()->addElement(0, 0, candleSticksAxisRect_);
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

    // create two bar plottables, for positive (green) and negative (red) volume bars:
    volumeBarsGraph_ = new QCPBars(volumeAxisRect_->axis(QCPAxis::atBottom), volumeAxisRect_->axis(QCPAxis::atLeft));
    // set the width of each bar to match the candle sticks
    volumeBarsGraph_->setWidth(candleSticksGraph_->width());
    volumeBarsGraph_->setWidthType(QCPBars::WidthType::wtPlotCoords);
    volumeBarsGraph_->setPen(Qt::NoPen);
    volumeBarsGraph_->setBrush(QColor(30, 144, 255));
    volumeBarsDataContainer_ = QSharedPointer<QCPBarsDataContainer>(new QCPBarsDataContainer);
    //sentinel used to replace the last item without a non 0 condition
    volumeBarsDataContainer_->add(QCPBarsData(0, 0));
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
            bool newCandle = candleMaker_.getClosingCandle(plotData_->ticks[lastPlotDataIndex_], currentCandle_);

            //the key for the plots are currently the index, but should change to time later
            candleBarsDataContainer_->set(candleBarsDataContainer_->size() - 1, QCPFinancialData(candleBarsDataContainer_->size() - 1, currentCandle_.open, currentCandle_.high, currentCandle_.low, currentCandle_.close));
            volumeBarsDataContainer_->set(volumeBarsDataContainer_->size() - 1, QCPBarsData(volumeBarsDataContainer_->size() - 1, currentCandle_.volume));

            if(newCandle)
            {
                candleBarsDataContainer_->add(QCPFinancialData(candleBarsDataContainer_->size(), currentCandle_.open, currentCandle_.high, currentCandle_.low, currentCandle_.close));
                volumeBarsDataContainer_->add(QCPBarsData(candleBarsDataContainer_->size(), currentCandle_.volume));
            }
        }

        if(autoScale_)
        {
            plot_->replot();
            candleSticksGraph_->rescaleAxes();
            volumeBarsGraph_->rescaleAxes();
        }

        if(!plotActive_)
        {
            replotTimer_->stop();
        }
    }
}

void TheTradingMachineTab::xAxisChanged(QCPRange range)
{
    if((floor(range.lower) <= 0 && ceil(range.upper) >= candleSticksGraph_->data()->size()))
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


