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
    volumeAxisRect_(nullptr)
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
    candleSticksGraph_ = new QCPFinancial(candleSticksAxisRect_->axis(QCPAxis::atBottom), candleSticksAxisRect_->axis(QCPAxis::atLeft));
    // sentinel element so that we can always replace the previous element instead of adding an if statement
    // which will be evaluated every time
    candleBarsDataContainer_ = QSharedPointer<QCPFinancialDataContainer>(new QCPFinancialDataContainer);
    candleBarsDataContainer_->add(QCPFinancialData(0, 0, 0, 0, 0));
    candleSticksGraph_->setData(candleBarsDataContainer_);
    plot_->plotLayout()->addElement(0, 0, candleSticksAxisRect_);

    //set up Volume graph
    volumeAxisRect_ = new QCPAxisRect(plot_);
    volumeAxisRect_->setRangeDrag(Qt::Horizontal);
    volumeAxisRect_->setRangeZoom(Qt::Horizontal);
    volumeAxisRect_->setMaximumSize(QSize(QWIDGETSIZE_MAX, 100));
    volumeAxisRect_->axis(QCPAxis::atBottom)->setLayer("axes");
    volumeAxisRect_->axis(QCPAxis::atBottom)->grid()->setLayer("grid");

    // create two bar plottables, for positive (green) and negative (red) volume bars:
    volumePositiveBarsGraph_ = new QCPBars(volumeAxisRect_->axis(QCPAxis::atBottom), volumeAxisRect_->axis(QCPAxis::atLeft));
    volumeNegativeBarsGraph_ = new QCPBars(volumeAxisRect_->axis(QCPAxis::atBottom), volumeAxisRect_->axis(QCPAxis::atLeft));

    volumePositiveBarsGraph_->setWidth(3600*4);
    volumePositiveBarsGraph_->setPen(Qt::NoPen);
    volumePositiveBarsGraph_->setBrush(QColor(100, 180, 110));
    //volumeNegativeBarsGraph_->setWidth(3600*4);
    volumeNegativeBarsGraph_->setPen(Qt::NoPen);
    volumeNegativeBarsGraph_->setBrush(QColor(180, 90, 90));
    volumePositiveBarsContainer_ = QSharedPointer<QCPBarsDataContainer>(new QCPBarsDataContainer);
    volumeNegativeBarsContainer_ = QSharedPointer<QCPBarsDataContainer>(new QCPBarsDataContainer);
    volumePositiveBarsGraph_->setData(volumePositiveBarsContainer_);
    volumeNegativeBarsGraph_->setData(volumeNegativeBarsContainer_);
    plot_->plotLayout()->addElement(1, 0, volumeAxisRect_);

    // bring bottom and main axis rect closer together:
    plot_->plotLayout()->setRowSpacing(0);
    volumeAxisRect_->setAutoMargins(QCP::msLeft|QCP::msRight|QCP::msBottom);
    volumeAxisRect_->setMargins(QMargins(0, 0, 0, 0));

    // make axis rects' left side line up:
    QCPMarginGroup *group = new QCPMarginGroup(plot_);
    candleSticksAxisRect_->setMarginGroup(QCP::msLeft|QCP::msRight, group);
    volumeAxisRect_->setMarginGroup(QCP::msLeft|QCP::msRight, group);

//    // interconnect x axis ranges of main and bottom axis rects:
//    connect(plot_->xAxis, SIGNAL(rangeChanged(QCPRange)), volumeAxisRect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
//    connect(volumeAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), plot_->xAxis, SLOT(setRange(QCPRange)));

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

    qDebug("constructor done");
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

            candleBarsDataContainer_->set(candleBarsDataContainer_->size() - 1, QCPFinancialData(candleBarsDataContainer_->size() - 1, currentCandle_.open, currentCandle_.high, currentCandle_.low, currentCandle_.close));

            if(newCandle)
                candleBarsDataContainer_->add(QCPFinancialData(candleBarsDataContainer_->size(), currentCandle_.open, currentCandle_.high, currentCandle_.low, currentCandle_.close));
        }
        plot_->replot();
        candleSticksGraph_->rescaleAxes();

        if(finished)
        {
            replotTimer_->stop();
        }
    }
}



