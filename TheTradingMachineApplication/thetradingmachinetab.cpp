#include "thetradingmachinetab.h"
#include "CandleMaker.h"

TheTradingMachineTab::TheTradingMachineTab(const AlgorithmApi& api, IBInterfaceClient* client, QWidget* parent) :
    QWidget(parent),
    gridLayout_(nullptr),
    plot_(nullptr),
    replotTimer_(new QTimer(this)),
    candleMaker_(60),
    plotData_(nullptr),
    lastPlotDataIndex_(0),
    api_(api),
    client_(client),
    candleSticksAxisRect_(nullptr),
    candleSticksGraph_(nullptr),
    volumeAxisRect_(nullptr),
    volumeBarsGraph_(nullptr)
{
    this->setObjectName(QStringLiteral("tab"));
    gridLayout_ = new QGridLayout(this);
    gridLayout_->setObjectName(QStringLiteral("gridLayout"));
    plot_ = new QCustomPlot(this);
    plot_->plotLayout()->clear(); //remove all layouts so we can start from scratch
    plot_->setObjectName(QStringLiteral("plot"));
    plot_->setInteraction(QCP::iRangeDrag);
    plot_->setInteraction(QCP::iRangeZoom);
    gridLayout_->addWidget(plot_, 0, 0, 1, 1);


    //set up Candle Stick Graph
    candleSticksAxisRect_ = new QCPAxisRect(plot_);
    candleSticksAxisRect_->setRangeDrag(Qt::Horizontal);
    candleSticksAxisRect_->setRangeZoom(Qt::Horizontal);
    candleSticksGraph_ = new QCPGraph(candleSticksAxisRect_->axis(QCPAxis::atBottom), candleSticksAxisRect_->axis(QCPAxis::atLeft));
    candleSticksGraph_->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    candleSticksGraph_->setLineStyle(QCPGraph::lsNone);
    plot_->plotLayout()->addElement(0, 0, candleSticksAxisRect_);

    //set up Volume graph
    volumeAxisRect_ = new QCPAxisRect(plot_);
    volumeAxisRect_->setRangeDrag(Qt::Horizontal);
    volumeAxisRect_->setRangeZoom(Qt::Horizontal);
    volumeBarsGraph_ = new QCPBars(volumeAxisRect_->axis(QCPAxis::atBottom), volumeAxisRect_->axis(QCPAxis::atLeft));
    plot_->plotLayout()->addElement(1, 0, volumeAxisRect_);

    candleDataContainer_ = QSharedPointer<QCPGraphDataContainer>(new QCPGraphDataContainer);
    // sentinel element so that we can always replace the previous element instead of adding an if statement
    // which will be evaluated every time
    candleDataContainer_->add(QCPGraphData(0, 0));
    candleSticksGraph_->setData(candleDataContainer_);

//    plot_->legend->setVisible(true);

//    // create bottom axis rect for volume bar chart:
//    volumeAxisRect = new QCPAxisRect(plot_);

//    volumeAxisRect->setRangeDrag(Qt::Horizontal);
//    volumeAxisRect->setRangeZoom(Qt::Horizontal);
//    plot_->plotLayout()->addElement(1, 0, volumeAxisRect);
//    volumeAxisRect->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
//    volumeAxisRect->axis(QCPAxis::atBottom)->setLayer("axes");
//    volumeAxisRect->axis(QCPAxis::atBottom)->grid()->setLayer("grid");

//    // bring bottom and main axis rect closer together:
//    plot_->plotLayout()->setRowSpacing(0);
//    volumeAxisRect->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
//    volumeAxisRect->setMargins(QMargins(0, 0, 0, 0));
//    // create two bar plottables, for positive (green) and negative (red) volume bars:
//    plot_->setAutoAddPlottableToLegend(false);
//    volumeBarsPos = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));
//    volumeBarsNeg = new QCPBars(volumeAxisRect->axis(QCPAxis::atBottom), volumeAxisRect->axis(QCPAxis::atLeft));

//    volumeBarsPos->setWidthType(QCPBars::WidthType::wtAbsolute);
//    volumeBarsPos->setWidth(10);
//    volumeBarsPos->setPen(Qt::NoPen);
//    volumeBarsPos->setBrush(QColor(100, 180, 110));
//    volumeBarsNeg->setWidth(3600*4);
//    volumeBarsNeg->setPen(Qt::NoPen);
//    volumeBarsNeg->setBrush(QColor(180, 90, 90));

//    // interconnect x axis ranges of main and bottom axis rects:
//    connect(plot_->xAxis, SIGNAL(rangeChanged(QCPRange)), volumeAxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
//    connect(volumeAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), plot_->xAxis, SLOT(setRange(QCPRange)));

//    // make axis rects' left side line up:
//    QCPMarginGroup *group = new QCPMarginGroup(plot_);
//    plot_->axisRect()->setMarginGroup(QCP::msLeft|QCP::msRight, group);
//    volumeAxisRect->setMarginGroup(QCP::msLeft|QCP::msRight, group);

    //prompt user for the input method. real time or historical ticks
    std::string fpTest("..\\outputfiles\\Jul 17NVDA.tickdat");
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
//    if(!api_.stopAlgorithm(algorithmHandle_))
//    {
//        qDebug("Unable to stop algorithm!!!");
//        assert(false);
//    }
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
        bool finished = plotData_->finished;
        // according to stl, "Concurrently accessing or modifying different elements is safe."
        // as long as other thread is always pushing to the end and we are accessing the middle,
        // the rule is satisfied
        lock.unlock();

        for(; lastPlotDataIndex_ < plotDataSz; ++lastPlotDataIndex_)
        {
            bool newCandle = candleMaker_.getClosingCandle(plotData_->ticks[lastPlotDataIndex_], currentCandle_);

            candleDataContainer_->set(candleDataContainer_->size() - 1, QCPGraphData(candleDataContainer_->size() - 1, currentCandle_.close));

            if(newCandle)
                candleDataContainer_->add(QCPGraphData(candleDataContainer_->size(), currentCandle_.close));

            //candleSticksGraph_->setData(candleDataContainer_);
            //candleSticksGraph_->addData(lastPlotDataIndex_, plotData_->ticks[lastPlotDataIndex_].price);
            //volumeBarsGraph_->addData(lastPlotDataIndex_, plotData_->ticks[lastPlotDataIndex_].price);
        }
        plot_->replot();
        candleSticksGraph_->rescaleAxes();

        if(finished)
        {
            replotTimer_->stop();
        }
    }
}



