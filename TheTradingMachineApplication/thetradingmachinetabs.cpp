#include "thetradingmachinetabs.h"

TheTradingMachineTabs::TheTradingMachineTabs(QWidget* parent) :
    QWidget(parent),
    gridLayout_(nullptr),
    horizontalScrollBar_(nullptr),
    verticalScrollBar_(nullptr),
    plot_(nullptr),
    plotData_(nullptr),
    lastPlotDataIndex_(0),
    replotTimer_(new QTimer(this))
{
    this->setObjectName(QStringLiteral("tab"));
    gridLayout_ = new QGridLayout(this);
    gridLayout_->setObjectName(QStringLiteral("gridLayout"));
    horizontalScrollBar_ = new QScrollBar(this);
    horizontalScrollBar_->setObjectName(QStringLiteral("horizontalScrollBar"));
    horizontalScrollBar_->setOrientation(Qt::Horizontal);

    gridLayout_->addWidget(horizontalScrollBar_, 1, 0, 1, 1);

    plot_ = new QCustomPlot(this);
    plot_->setObjectName(QStringLiteral("plot"));
    plot_->addGraph();

    gridLayout_->addWidget(plot_, 0, 0, 1, 1);

    verticalScrollBar_ = new QScrollBar(this);
    verticalScrollBar_->setObjectName(QStringLiteral("verticalScrollBar"));
    verticalScrollBar_->setOrientation(Qt::Vertical);

    gridLayout_->addWidget(verticalScrollBar_, 0, 1, 1, 1);
}

TheTradingMachineTabs::~TheTradingMachineTabs()
{
    qDebug("destruct tab");
}

void TheTradingMachineTabs::playPlotData(int instHandle, std::shared_ptr<PlotData> plotdata)
{
    qDebug("playPlotData");
    plotData_ = plotdata;
    algorithmHandle_ = instHandle;

    connect(replotTimer_, &QTimer::timeout, this, &TheTradingMachineTabs::updatePlot);
    replotTimer_->start(0);
}

int TheTradingMachineTabs::getHandle()
{
    return algorithmHandle_;
}

void TheTradingMachineTabs::updatePlot(void)
{
    std::unique_lock<std::mutex> lock(plotData_->plotDataMtx, std::defer_lock);

    // if gui was running on a faster thread for some reason than the algorithm
    // then simply let the gui do something else while the algorithm is operating
    // on the plotData buffer. neither side has to block
    if(lock.try_lock())
    {
        const size_t plotDataSz = plotData_->ticks.size();
        bool finished = plotData_->finished;
        lock.unlock();

        for(; lastPlotDataIndex_ < plotDataSz; ++lastPlotDataIndex_)
        {
            plot_->graph()->addData(lastPlotDataIndex_, plotData_->ticks[lastPlotDataIndex_].price);
        }
        plot_->replot();
        plot_->rescaleAxes();
        if(finished)
        {
            replotTimer_->stop();
        }
    }
}



