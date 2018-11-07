#include "candleplot.h"
#include "indicatorplot.h"
#include "../Indicators/Indicators/Indicators.h"
#include "indicatordialog.h"
#include <unordered_set>
#include <memory>

CandlePlot::CandlePlot(QCustomPlot& parentPlot, QCPAxisRect& axisRect):
    QObject (&axisRect),
    parentPlot_(parentPlot),
    axisRect_(axisRect),
    candleBars_(new QCPFinancial(axisRect.axis(QCPAxis::atBottom), axisRect.axis(QCPAxis::atLeft))),
    dataContainer_(new QCPFinancialDataContainer),
    size_(0)
{
    candleBars_->setData(dataContainer_);
    candleBars_->setWidthType(QCPFinancial::WidthType::wtPlotCoords);
    candleBars_->setWidth(60);


    parentPlot_.setContextMenuPolicy(Qt::CustomContextMenu);
    connect(&parentPlot_, &QCustomPlot::customContextMenuRequested, this, &CandlePlot::menuShowSlot);
}

CandlePlot::~CandlePlot()
{

}

void CandlePlot::updatePlotAdd(const time_t candleTime, const Bar &candle)
{
    // add a new bar volume and candlesticks
    dataContainer_->add(QCPFinancialData(candleTime, candle.open, candle.high, candle.low, candle.close));
    ++size_;

    // since activeIndicatorPlots_ entires map plottables to
    // iplots, we can have more than one plottable mapped to the same
    // iplot. for example, macd has 3 values which can be mapped
    // to the same indicator. we use this unordered_set to mark
    // which has been updated as we traverse our entries
    std::unordered_set<std::shared_ptr<IPlot>> updatedIndicators;

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
        dataContainer_->set(size_ - 1, QCPFinancialData(candleTime , candle.open, candle.high, candle.low, candle.close));

        std::unordered_set<std::shared_ptr<IPlot>> updatedIndicators;
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

void CandlePlot::pastCandlesPlotUpdate(std::shared_ptr<IPlot> iplot)
{
    // keep the indicatorPlot up to date with all the candles we currently have
    for(auto& it: *dataContainer_)
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

void CandlePlot::rescaleValueAxisAutofit()
{
    candleBars_->rescaleValueAxis(false, true);
}

//void CandlePlot::addIndicator(std::unique_ptr<IPlot> indicatorPlot)
//{
//    // keep the indicatorPlot up to date with all the candles we currently have
//    for(auto& it: *dataContainer_)
//    {
//        switch(indicatorPlot->valueType)
//        {
//        case IPlot::ValueType::OPEN:
//            indicatorPlot->updatePlotAdd( static_cast<time_t>(it.key), it.open);
//            break;

//        case IPlot::ValueType::HIGH:
//            indicatorPlot->updatePlotAdd( static_cast<time_t>(it.key), it.high);
//            break;

//        case IPlot::ValueType::LOW:
//            indicatorPlot->updatePlotAdd( static_cast<time_t>(it.key), it.low);
//            break;

//        case IPlot::ValueType::CLOSE:
//            indicatorPlot->updatePlotAdd( static_cast<time_t>(it.key), it.close);
//            break;
//        default:
//            break;
//        }
//    }
//    //add this indicator to our list
//    activeIndicatorPlots_[indicatorType].push_back(std::move(indicatorPlot));
//}

void CandlePlot::indicatorSelected(QPoint pos)
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
        qDebug("Bollinger Bands");
        //prompt user
        IndicatorDialog diag;
        diag.exec();
        indicatorLaunch<BollingerBands>(OhlcType::CLOSE, 5, 3);
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
    subMenu->addAction("Exponential Moving Average");
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
    subMenu->addAction("Relative Strength Index");
    subMenu = menu->addMenu("S");
    subMenu->addAction("Simple Moving Average", this, [](){});
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

void CandlePlot::removePlottable(QCPAbstractPlottable *plottable)
{
    auto iPlotFindIt = activeIndicatorPlots_.find(plottable);
    if(iPlotFindIt != activeIndicatorPlots_.end())
    {
        auto iplot = iPlotFindIt->second;

        //find the other associated plottables. eg macd has 3 and bollinger has 2
        auto plottables = iplot->getPlottables();

        //remove the entries of these plottables
        for(const auto& it: plottables)
        {
            // first remove these from the visible plot
            parentPlot_.removePlottable(it);

            // remove the entry
            activeIndicatorPlots_.erase(it);
        }
    }
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
       indicatorSelected(pos);
    }
    else
    {
        // we disabled multiple selection for now
        removePlottable(selectedPlottables.first());
    }
}

template<typename IndicatorType, typename... Args>
void CandlePlot::indicatorLaunch(OhlcType valueType, Args... args)
{

    std:: cout << std::forward

//    auto smaIndicator = std::make_unique<IndicatorType>(std::forward<Args>(args)...);
//    auto smaPlot = std::make_shared<IndicatorPlot<IndicatorType>>(axisRect_, std::move(smaIndicator), valueType);
//    pastCandlesPlotUpdate(smaPlot);
//    auto plottables = smaPlot->getPlottables();

//    // add these plottables to our iplot map
//    for(const auto& plottablesIt: plottables)
//    {
//        activeIndicatorPlots_[plottablesIt] = smaPlot;
//    }
}

//template<typename IndicatorType, typename... Args>
//void CandlePlot::indicatorLaunch(Args... args)
//{
//    auto indicator = std::make_unique<IndicatorType>(std::forward<Args>(args)...);
//    auto indicatorPlot = std::make_shared<IndicatorPlot<IndicatorType>>(axisRect_, std::move(indicator)); //default value type
//    pastCandlesPlotUpdate(indicatorPlot);
//    auto plottables = indicatorPlot->getPlottables();

//    // add these plottables to our iplot map
//    for(const auto& plottablesIt: plottables)
//    {
//        activeIndicatorPlots_[plottablesIt] = indicatorPlot;
//    }
//}
