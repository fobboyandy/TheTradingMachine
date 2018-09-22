#include "thetradingmachinetabs.h"

//static class instantiation
QTimer TheTradingMachineTabs::replotTimer_;

TheTradingMachineTabs::TheTradingMachineTabs(QWidget* parent) :
    QWidget(parent),
    gridLayout_(nullptr),
    horizontalScrollBar_(nullptr),
    verticalScrollBar_(nullptr),
    plot_(nullptr),
    plotData_(nullptr)

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

    gridLayout_->addWidget(plot_, 0, 0, 1, 1);

    verticalScrollBar_ = new QScrollBar(this);
    verticalScrollBar_->setObjectName(QStringLiteral("verticalScrollBar"));
    verticalScrollBar_->setOrientation(Qt::Vertical);

    gridLayout_->addWidget(verticalScrollBar_, 0, 1, 1, 1);
}

TheTradingMachineTabs::~TheTradingMachineTabs()
{
    qDebug("destruct tab");
    //parent will free
}

QCustomPlot* TheTradingMachineTabs::plot()
{
    return plot_;
}

void TheTradingMachineTabs::setPlotData(int instHandle, const PlotData* plotdata)
{
    plotData_ = plotdata;
    algorithmHandle_ = instHandle;
}

void TheTradingMachineTabs::run()
{
    //starts the timer if it hasn't been started. otherwise
    // it gets restarted
    replotTimer_.start(0);
    connect(&replotTimer_, &QTimer::timeout, this, &TheTradingMachineTabs::updatePlot);
}

int TheTradingMachineTabs::getHandle()
{
    return algorithmHandle_;
}

void TheTradingMachineTabs::updatePlot()
{
    //qDebug("timer fired");
}


