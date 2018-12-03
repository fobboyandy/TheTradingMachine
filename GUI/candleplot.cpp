#include "candleplot.h"
#include "indicatorplot.h"
#include "indicatordialog.h"
#include "indicatorincludes.h"
#include <unordered_set>
#include <memory>

CandlePlot::CandlePlot(QCustomPlot& parentPlot):
    BasePlot(parentPlot)
{
    candleBars_ = new QCPFinancial(axisRect_.axis(QCPAxis::atBottom), axisRect_.axis(QCPAxis::atLeft));
    candleBars_->setWidthType(QCPFinancial::WidthType::wtPlotCoords);
    candleBars_->setWidth(60);

    parentPlot_.setContextMenuPolicy(Qt::CustomContextMenu);
    connect(&parentPlot_, &QCustomPlot::customContextMenuRequested, this, &CandlePlot::menuShowSlot);

    size_ = 0;
}

CandlePlot::~CandlePlot()
{
}

void CandlePlot::updatePlotAdd(const time_t candleTime, const Bar &candle)
{
    // add a new bar
    candleBars_->addData(candleTime, candle.open, candle.high, candle.low, candle.close);
    ++size_;

    // since activeIndicatorPlots_ entires map plottables to
    // iplots, we can have more than one plottable mapped to the same
    // iplot. for example, macd has 3 values which can be mapped
    // to the same indicator. we use this unordered_set to mark
    // which has been updated as we traverse our entries
    std::unordered_set<std::shared_ptr<IIndicatorPlot>> updatedIndicators;

    // update all the indicators
    for(auto& activePlotIt: activeIndicatorPlots_)
    {
        if(updatedIndicators.find(activePlotIt.second) == updatedIndicators.end())
        {
            switch(activePlotIt.second->valueType)
            {
            case OhlcType::OPEN:
                activePlotIt.second->updatePlotAdd( candleTime, candle.open);
                break;

            case OhlcType::HIGH:
                activePlotIt.second->updatePlotAdd( candleTime, candle.high);
                break;

            case OhlcType::LOW:
                activePlotIt.second->updatePlotAdd( candleTime, candle.low);
                break;

            // close values as default
            case OhlcType::CLOSE:
            default:
                activePlotIt.second->updatePlotAdd( candleTime, candle.close);
                break;
            }

            // mark as updated
            updatedIndicators.insert(activePlotIt.second);
        }
    }
}

void CandlePlot::updatePlotReplace(const time_t candleTime, const Bar &candle)
{
    if(size_ > 0)
    {
        candleBars_->data()->set(size_ - 1, QCPFinancialData(candleTime , candle.open, candle.high, candle.low, candle.close));

        std::unordered_set<std::shared_ptr<IIndicatorPlot>> updatedIndicators;
        // update all the indicators belonging to this plot
        for(auto& activePlotIt: activeIndicatorPlots_)
        {
            if(updatedIndicators.find(activePlotIt.second) == updatedIndicators.end() )
            {
                switch(activePlotIt.second->valueType)
                {
                case OhlcType::OPEN:
                    activePlotIt.second->updatePlotReplace( candleTime, candle.open);
                    break;

                case OhlcType::HIGH:
                    activePlotIt.second->updatePlotReplace( candleTime, candle.high);
                    break;

                case OhlcType::LOW:
                    activePlotIt.second->updatePlotReplace( candleTime, candle.low);
                    break;

                // use closing value as default
                case OhlcType::CLOSE:
                default:
                    activePlotIt.second->updatePlotReplace( candleTime, candle.close);
                    break;
                }

                // mark as updated
                updatedIndicators.insert(activePlotIt.second);
            }
        }
    }
}


// keep the indicatorPlot up to date with all the candles we currently have
// this is used when we add an indicator after a graph has started
// for a while already. we still want to be able to plot the indicator for
// the previous candles
void CandlePlot::pastCandlesPlotUpdate(std::shared_ptr<IIndicatorPlot> iplot)
{
    for(auto& it: *candleBars_->data())
    {
        switch(iplot->valueType)
        {
        case OhlcType::OPEN:
            iplot->updatePlotAdd( static_cast<time_t>(it.key), it.open);
            break;

        case OhlcType::HIGH:
            iplot->updatePlotAdd( static_cast<time_t>(it.key), it.high);
            break;

        case OhlcType::LOW:
            iplot->updatePlotAdd( static_cast<time_t>(it.key), it.low);
            break;

        case OhlcType::CLOSE:
            iplot->updatePlotAdd( static_cast<time_t>(it.key), it.close);
            break;
        default:
            break;
        }
    }
}

void CandlePlot::rescalePlot()
{
    if(autoScaleKeyAxis_)
    {
        axisRect_.axis(QCPAxis::AxisType::atBottom)->rescale();
        axisRect_.axis(QCPAxis::AxisType::atLeft)->rescale();
    }
    else
    {
        candleBars_->rescaleValueAxis(false, true);

        // we need to rescale each graph with respect to
        // their axes. For each axis, we need to autofit
        // all (can be more than one) the graphs. We iterate
        // through the existing axies and find the max range
        // one by one.

        //initialize maxRange to the candle plot
        bool foundRange = false;
        QCP::SignDomain signDomain = QCP::sdBoth;
        if (axisRect_.axis(QCPAxis::atLeft)->scaleType() == QCPAxis::stLogarithmic)
        {
            signDomain = (axisRect_.axis(QCPAxis::atLeft)->range().upper < 0 ? QCP::sdNegative : QCP::sdPositive);
        }
        auto maxRange = candleBars_->getValueRange(foundRange, signDomain, axisRect_.axis(QCPAxis::atBottom)->range());
        //valueAxis->setRange(newRange);

        // get all our value axes
        auto axes = axisRect_.axes(QCPAxis::atLeft | QCPAxis::atRight);
        for(auto& axesIt: axes)
        {
            // graphs in this axis
            auto commonGraphs = axesIt->graphs();
            for(auto& commonGraphsIt : commonGraphs)
            {
                foundRange = false;
                signDomain = QCP::sdBoth;
                if (axisRect_.axis(QCPAxis::atLeft)->scaleType() == QCPAxis::stLogarithmic)
                {
                    signDomain = (axisRect_.axis(QCPAxis::atLeft)->range().upper < 0 ? QCP::sdNegative : QCP::sdPositive);
                }

                // find the range for the current visible key range
                auto graphRange = commonGraphsIt->getValueRange(foundRange, signDomain, axisRect_.axis(QCPAxis::atBottom)->range());

                // find the max range
                if(graphRange.lower < maxRange.lower)
                {
                    maxRange.lower = graphRange.lower;
                }

                if(graphRange.upper > maxRange.upper)
                {
                    maxRange.upper = graphRange.upper;
                }
            }

            // rescale the current axis to the max range
            axesIt->setRange(maxRange);
        }
    }
}

void CandlePlot::indicatorSelectionMenu(QPoint pos)
{
    QMenu* menu = new QMenu();
    // destroy the menu after closing
    menu->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);

    // dynamically create a new graph each time based on the state of the plots
    // and indicators
    auto subMenu = menu->addMenu("A");
    subMenu->addAction("Absolute Price Oscillator");
    subMenu->addAction("Accumulation/Distribution Line");
    subMenu->addAction("Accumulation/Distribution Oscillator");
    subMenu->addAction("Annualized Historical Volatility");
    subMenu->addAction("Aroon");
    subMenu->addAction("Aroon Oscillator");
    subMenu->addAction("Average Directional Movement Index");
    subMenu->addAction("Average Directional Movement Rating");
    subMenu->addAction("Average Price");
    subMenu->addAction("Average True Range");
    subMenu->addAction("Awesome Oscillator");
    subMenu = menu->addMenu("B");
    subMenu->addAction("Balance of Power");
    subMenu->addAction("Bollinger Bands", this, [this]()
    {
        //prompt user
        IndicatorDialog diag;
        diag.addSpinbox("Period", 5, 1);
        diag.addSpinbox("Standard Deviation Factor", 3, 1);
        diag.exec();

        //if user pressed OK
        if(diag.valid())
        {
            auto period = diag.getSpinboxValue("Period");
            auto scale = diag.getSpinboxValue("Standard Deviation Factor");
            indicatorLaunch<BollingerBands>(OhlcType::CLOSE, IndicatorDisplayType::OVERLAY, period, scale);
        }
    });
    subMenu->addAction("Chaikins Volatility");
    subMenu = menu->addMenu("C");
    subMenu->addAction("Chande Momentum Oscillator");
    subMenu->addAction("Commodity Channel Index");
    subMenu->addAction("Crossany");
    subMenu->addAction("Crossover");
    subMenu = menu->addMenu("D");
    subMenu->addAction("Detrended Price Oscillator");
    subMenu->addAction("Directional Indicator");
    subMenu->addAction("Directional Movement");
    subMenu->addAction("Directional Movement Index");
    subMenu->addAction("Double Exponential Moving Average");
    subMenu = menu->addMenu("E");
    subMenu->addAction("Ease of Movement");
    subMenu->addAction("Exponential Decay");
    subMenu->addAction("Exponential Moving Average", [this]()
    {
        //prompt user
        IndicatorDialog diag;
        diag.addSpinbox("Period", 5, 1);
        diag.exec();

        //if user pressed OK
        if(diag.valid())
        {
            auto period = diag.getSpinboxValue("Period");
            qDebug(std::to_string(period).c_str());
            indicatorLaunch<ExponentialMovingAverage>(OhlcType::CLOSE, IndicatorDisplayType::OVERLAY, period);
        }
    });
    subMenu = menu->addMenu("F");
    subMenu->addAction("Fisher Transform");
    subMenu->addAction("Forecast Oscillator");
    subMenu = menu->addMenu("H");
    subMenu->addAction("Hull Moving Average");
    subMenu = menu->addMenu("A");
    subMenu->addAction("Kaufman Adaptive Moving Average");
    subMenu->addAction("Klinger Volume Oscillator");
    subMenu = menu->addMenu("L");
    subMenu->addAction("Lag");
    subMenu->addAction("Linear Decay");
    subMenu->addAction("Linear Regression");
    subMenu->addAction("Linear Regression Intercept");
    subMenu->addAction("Linear Regression Slope");
    subMenu = menu->addMenu("M");
    subMenu->addAction("Market Facilitation Index");
    subMenu->addAction("Mass Index");
    subMenu->addAction("Maximum In Period");
    subMenu->addAction("Mean Deviation Over Period");
    subMenu->addAction("Median Price");
    subMenu->addAction("Mesa Sine Wave");
    subMenu->addAction("Minimum In Period");
    subMenu->addAction("Momentum");
    subMenu->addAction("Money Flow Index");
    subMenu->addAction("Moving Average Convergence/Divergence");
    subMenu = menu->addMenu("N");
    subMenu->addAction("Negative Volume Index");
    subMenu->addAction("Normalized Average True Range");
    subMenu = menu->addMenu("O");
    subMenu->addAction("On Balance Volume");
    subMenu = menu->addMenu("P");
    subMenu->addAction("Parabolic SAR");
    subMenu->addAction("Percentage Price Oscillator");
    subMenu->addAction("Positive Volume Index");
    subMenu = menu->addMenu("Q");
    subMenu->addAction("Qstick");
    subMenu = menu->addMenu("R");
    subMenu->addAction("Rate of Change");
    subMenu->addAction("Rate of Change Ratio");
    subMenu->addAction("Relative Strength Index", this, [this]()
    {
        //prompt user
        IndicatorDialog diag;
        diag.addSpinbox("Period", 5, 1);
        diag.exec();

        //if user pressed OK
        if(diag.valid())
        {
            auto period = diag.getSpinboxValue("Period");
            indicatorLaunch<RelativeStrengthIndex>(OhlcType::CLOSE, IndicatorDisplayType::INDICATOR, period);
        }
    });
    subMenu = menu->addMenu("S");
    subMenu->addAction("Simple Moving Average", this, [this]()
    {
        //prompt user
        IndicatorDialog diag;
        diag.addSpinbox("Period", 5, 1);
        diag.exec();

        //if user pressed OK
        if(diag.valid())
        {
            auto period = diag.getSpinboxValue("Period");
            indicatorLaunch<SimpleMovingAverage>(OhlcType::CLOSE, IndicatorDisplayType::OVERLAY, period);
        }
    });
    subMenu->addAction("Standard Deviation Over Period");
    subMenu->addAction("Standard Error Over Period");
    subMenu->addAction("Stochastic Oscillator");
    subMenu->addAction("Stochastic RSI");
    subMenu->addAction("Sum Over Period");
    subMenu = menu->addMenu("T");
    subMenu->addAction("Time Series Forecast");
    subMenu->addAction("Triangular Moving Average");
    subMenu->addAction("Triple Exponential Moving Average");
    subMenu->addAction("Trix");
    subMenu->addAction("True Range");
    subMenu->addAction("Typical Price");
    subMenu = menu->addMenu("U");
    subMenu->addAction("Ultimate Oscillator");
    subMenu = menu->addMenu("V");
    subMenu->addAction("Variable Index Dynamic Average");
    subMenu->addAction("Variance Over Period");
    subMenu->addAction("Vector Absolute Value");
    subMenu->addAction("Vector Addition");
    subMenu->addAction("Vector Arccosine");
    subMenu->addAction("Vector Arcsine");
    subMenu->addAction("Vector Arctangent");
    subMenu->addAction("Vector Base-10 Log");
    subMenu->addAction("Vector Ceiling");
    subMenu->addAction("Vector Cosine");
    subMenu->addAction("Vector Degree Conversion");
    subMenu->addAction("Vector Division");
    subMenu->addAction("Vector Exponential");
    subMenu->addAction("Vector Floor");
    subMenu->addAction("Vector Hyperbolic Cosine");
    subMenu->addAction("Vector Hyperbolic Sine");
    subMenu->addAction("Vector Hyperbolic Tangent");
    subMenu->addAction("Vector Multiplication");
    subMenu->addAction("Vector Natural Log");
    subMenu->addAction("Vector Radian Conversion");
    subMenu->addAction("Vector Round");
    subMenu->addAction("Vector Sine");
    subMenu->addAction("Vector Square Root");
    subMenu->addAction("Vector Subtraction");
    subMenu->addAction("Vector Tangent");
    subMenu->addAction("Vector Truncate");
    subMenu->addAction("Vertical Horizontal Filter");
    subMenu->addAction("Volume Oscillator");
    subMenu->addAction("Volume Weighted Moving Average");
    subMenu = menu->addMenu("W");
    subMenu->addAction("Weighted Close Price");
    subMenu->addAction("Weighted Moving Average");
    subMenu->addAction("Wilders Smoothing");
    subMenu->addAction("Williams %R");
    subMenu->addAction("Williams Accumulation/Distribution");
    subMenu = menu->addMenu("Z");
    subMenu->addAction("Zero-Lag Exponential Moving Average");

    menu->popup(parentPlot_.mapToGlobal(pos));
}

void CandlePlot::removeIndicatorMenu(QPoint pos, QList<QCPAbstractPlottable*> plottables)
{
    QMenu* menu = new QMenu();
    // destroy the menu after closing
    menu->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);

    menu->addAction("Remove Indicator", this, [this, plottables]()
    {
        // get the value axis shared by these plots. check the number
        // of remaining plottables after removing them
        auto commonValueAxis = plottables.front()->valueAxis();

        // all plottables associated with a chosen iplot
        // are provided by the argument
        for(auto& plottable: plottables)
        {
            // erase the plot entries
            activeIndicatorPlots_.erase(plottable);

            // remove it from the graph
            parentPlot_.removePlottable(plottable);

        }

        // if there are no more plottables associated with
        // this axis, remove the axis
        if(commonValueAxis->plottables().size() == 0)
        {
            axisRect_.removeAxis(commonValueAxis);
        }
    });

    menu->popup(parentPlot_.mapToGlobal(pos));
}

double CandlePlot::lowerRange()
{
    return candleBars_->data()->at(0)->mainKey();
}

double CandlePlot::upperRange()
{
    return candleBars_->data()->at(candleBars_->data()->size() - 1)->mainKey();
}

int CandlePlot::size()
{
    return size_;
}

void CandlePlot::menuShowSlot(QPoint pos)
{
    auto selectedPlottables = parentPlot_.selectedPlottables();
    // if no plottables were selected then show indicator selection
    if(selectedPlottables.size() == 0)
    {
        indicatorSelectionMenu(pos);
    }
    else
    {
        removeIndicatorMenu(pos, selectedPlottables);
    }
}

void CandlePlot::plotSelectSlot(bool selected)
{
    // mark the other graphs as selected as well
    if(selected)
    {
        std::shared_ptr<IIndicatorPlot> selectedPlot;

        // find the graph that is currently selected
        for(auto& plottableEntry: activeIndicatorPlots_)
        {
            if(plottableEntry.first->selected())
            {
                selectedPlot = plottableEntry.second;
                break; // we know only one will be selected
            }
        }

        auto plottables = selectedPlot->getPlottables();
        for(auto& plottable: plottables)
        {
            // cast to qcpgraph since we only have qcpgraphs at the moment
            plottable->setSelection(QCPDataSelection(static_cast<QCPGraph*>(plottable)->data()->dataRange()));
        }
    }
}

void CandlePlot::xAxisChanged(QCPRange range)
{
    // compare the range of our zoom with our data.
    // if the zoom contains all the data, then autoscale
    if(floor(range.lower) <= candleBars_->data()->at(0)->mainKey() &&
        ceil(range.upper) >= candleBars_->data()->at(candleBars_->data()->size() - 1)->mainKey())
    {
        autoScaleKeyAxis_ = true;
    }
    else
    {
        autoScaleKeyAxis_ = false;
    }

    //rescale our plot.
    rescalePlot();
}

template<typename IndicatorType, typename... Args>
void CandlePlot::indicatorLaunch(OhlcType valueType, IndicatorDisplayType displayType, Args... args)
{
    auto indicator = std::make_unique<IndicatorType>(args...);
    auto plot = std::make_shared<IndicatorPlot<IndicatorType>>(axisRect_, std::move(indicator), valueType, displayType);
    pastCandlesPlotUpdate(plot);
    auto plottables = plot->getPlottables();

    // add these plottables to our iplot map
    for(auto& plottable: plottables)
    {
        activeIndicatorPlots_[plottable] = plot;
        connect(plottable, SIGNAL(selectionChanged(bool)), this, SLOT(plotSelectSlot(bool)));
    }
}
