#include "thetradingmachinetab.h"

TheTradingMachineTab::TheTradingMachineTab(const AlgorithmApi& api, IBInterfaceClient* client, QWidget* parent) :
    QWidget(parent),
    gridLayout_(nullptr),
    plot_(nullptr),
    plotData_(nullptr),
    lastPlotDataIndex_(0),
    replotTimer_(new QTimer(this)),
    api_(api),
    client_(client)
{
    this->setObjectName(QStringLiteral("tab"));
    gridLayout_ = new QGridLayout(this);
    gridLayout_->setObjectName(QStringLiteral("gridLayout"));
    plot_ = new QCustomPlot(this);
    plot_->setObjectName(QStringLiteral("plot"));
    plot_->addGraph();

    gridLayout_->addWidget(plot_, 0, 0, 1, 1);

    //prompt user for the input method. real time or historical ticks
    std::string fpTest("D:\\Users\\fobboyandy\\Desktop\\TheTradingMachine\\outputfiles\\Jul 19AMD.tickdat");
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



