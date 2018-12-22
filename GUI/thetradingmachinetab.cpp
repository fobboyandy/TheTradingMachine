#include <iostream>
#include "thetradingmachinetab.h"
#include "playdialog.h"
#include "../BaseModules/Indicators/SimpleMovingAverage.h"
#include "indicatorgraph.h"
#include "annotationplot.h"

TheTradingMachineTab::TheTradingMachineTab(const AlgorithmApi& api, std::shared_ptr<InteractiveBrokersClient> client, QWidget* parent) :
    QWidget(parent),
    replotTimer_(new QTimer(this)),
    api_(api),
    client_(client),
    candleMaker_(60)
{
    valid_ = false;

    // prompt user for the input method. real time or historical ticks
    PlayDialog loadInput(this);
    loadInput.exec();
    auto input = loadInput.getInput();
    name_ = formatTabName(input);

    if(name_.size() == 0)
    {
        return;
    }

    //if real time, check for ib connection
    // instantiate the algorithm for this ticker
    algorithmHandle_ = api_.playAlgorithm(input.toStdString(), &plotData_, client_, false);
    if(algorithmHandle_ != -1)
    {
        // tab should only be valid if play algorithm and getplotdata worked
        valid_ = true;
        connect(replotTimer_, &QTimer::timeout, this, &TheTradingMachineTab::updatePlot);
    }

    // this sets up the widget grid and qcustomplot instance
    layoutSetup();

    // create basic plots
    plots_[0] = std::make_unique<CandlePlot>(*plot_);
    plots_[1] = std::make_unique<VolumePlot>(*plot_);
//    plots_.push_back(std::make_unique<AnnotationPlot>(*plot_));

    // initialize members here instead of the initializer list
    // to keep the initializer list shorter. shouldn't be too much
    // extra overhead
    autoScale_ = true;
    plotActive_ = false;
    lastPlotDataIndex_ = 0;
    lastAnnotationIndex_ = 0;

    // start the plotting
    replotTimer_->start(50);
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

    // make each plot take a new row so that they are aligned vertically
    plot_->plotLayout()->setFillOrder(QCPLayoutGrid::FillOrder::foColumnsFirst);
    plot_->plotLayout()->setWrap(1);

    // add our qcustomplot widget to the tab.
    gridLayout_->addWidget(plot_, 0, 0, 1, 1);
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

void TheTradingMachineTab::updatePlotNewCandle(const Candlestick &candle)
{
    for(auto plot: plots_)
    {
        plot.second->updatePlotAdd(candle);
    }
}

void TheTradingMachineTab::updatePlotReplaceCandle(const Candlestick &candle)
{
    for(auto plot: plots_)
    {
        plot.second->updatePlotReplace(candle);
    }
}

void TheTradingMachineTab::updatePlot(void)
{
    std::unique_lock<std::mutex> lock(plotData_->plotDataMtx);

    const size_t plotDataSz = plotData_->ticks.size();
    const size_t annotationDataSz = plotData_->annotations.size();
    // according to stl, "Concurrently accessing or modifying different elements is safe."
    // as long as other thread is always pushing to the end and we are accessing the middle,
    // the rule is satisfied
    lock.unlock();

    for(; lastPlotDataIndex_ < plotDataSz; ++lastPlotDataIndex_)
    {
        candleMaker_.addTick(plotData_->ticks[lastPlotDataIndex_]);
        auto closedCandles = candleMaker_.getClosedCandles();
        auto currentCandle = candleMaker_.getCurrentCandle();

        // for new closed candles, replaced the most recent candle
        // with the closed candle
        if(closedCandles.size() > 0)
        {
            // if there are new closed candles, we replace
            // the most recent candle with the first closed
            // candle
            updatePlotReplaceCandle(closedCandles[0]);

            // if there are more closed candles (this is possible
            // when the refresh rate of the plot is low)
            for(decltype(closedCandles.size()) i = 1; i < closedCandles.size(); ++i)
            {
                updatePlotNewCandle(closedCandles[i]);
            }

            // finally add in the current candle which will update with each new tick
            updatePlotNewCandle(currentCandle);
        }
        else
        {
            // if there are no new candles, simply update
            // the most recent candle with the updated tick
            updatePlotReplaceCandle(currentCandle);
        }

    }

    // add any new annotations from our user to the charts (for now only candle charts
    // have annotations)
    for(; lastAnnotationIndex_ < annotationDataSz; ++lastAnnotationIndex_)
    {

        const auto& annotation = plotData_->annotations[lastAnnotationIndex_];
        if(annotation != nullptr)
        {
            if(plots_.find(annotation->index_) == plots_.end())
            {
                plots_[annotation->index_] = std::make_unique<AnnotationPlot>(*plot_);
            }
            plots_[annotation->index_]->addAnnotation(annotation);
        }
    }

    // rescale all the plots according to their own defined scaling
    // function
    for(auto plot: plots_)
    {
        plot.second->rescalePlot();
    }

    //replot should always be happening to update the drawing
    plot_->replot();
}
