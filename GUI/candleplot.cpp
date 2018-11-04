#include "candleplot.h"

CandlePlot::CandlePlot(QCPAxisRect &axisRect):
    axisRect_(axisRect),
    candleBars_(new QCPFinancial(axisRect.axis(QCPAxis::atBottom), axisRect.axis(QCPAxis::atLeft))),
    dataContainer_(new QCPFinancialDataContainer),
    size_(0)
{
    candleBars_->setData(dataContainer_);
    candleBars_->setWidthType(QCPFinancial::WidthType::wtPlotCoords);
    candleBars_->setWidth(60);
}

CandlePlot::~CandlePlot()
{

}

void CandlePlot::updatePlotAdd(const time_t candleTime, const Bar &candle)
{
    // add a new bar volume and candlesticks
    dataContainer_->add(QCPFinancialData(candleTime, candle.open, candle.high, candle.low, candle.close));
    ++size_;

    // update all the indicators belonging to this plot
    for(auto& activePlotIt: activeIndicatorPlots_)
    {
        for(auto& plotIt: activePlotIt.second)
        {
            switch(plotIt->valueType)
            {
            case IPlot::ValueType::OPEN:
                plotIt->updatePlotAdd( candleTime, candle.open);
                break;

            case IPlot::ValueType::HIGH:
                plotIt->updatePlotAdd( candleTime, candle.high);
                break;

            case IPlot::ValueType::LOW:
                plotIt->updatePlotAdd( candleTime, candle.low);
                break;

            case IPlot::ValueType::CLOSE:
                plotIt->updatePlotAdd( candleTime, candle.close);
                break;
            default:
                break;
            }
        }
    }
}

void CandlePlot::updatePlotReplace(const time_t candleTime, const Bar &candle)
{
    if(size_ > 0)
    {
        dataContainer_->set(size_ - 1, QCPFinancialData(candleTime , candle.open, candle.high, candle.low, candle.close));
        // update all the indicators belonging to this plot
        for(auto& activePlotIt: activeIndicatorPlots_)
        {
            for(auto& plotIt: activePlotIt.second)
            {
                switch(plotIt->valueType)
                {
                case IPlot::ValueType::OPEN:
                    plotIt->updatePlotReplace( candleTime, candle.open);
                    break;

                case IPlot::ValueType::HIGH:
                    plotIt->updatePlotReplace( candleTime, candle.high);
                    break;

                case IPlot::ValueType::LOW:
                    plotIt->updatePlotReplace( candleTime, candle.low);
                    break;

                case IPlot::ValueType::CLOSE:
                    plotIt->updatePlotReplace( candleTime, candle.close);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void CandlePlot::rescaleValueAxisAutofit()
{
    candleBars_->rescaleValueAxis(false, true);
}

void CandlePlot::addIndicator(IPlot::IndicatorType indicatorType, std::unique_ptr<IPlot> indicatorPlot)
{
    // keep the indicatorPlot up to date with all the candles we currently have
    for(auto& it: *dataContainer_)
    {
        switch(indicatorPlot->valueType)
        {
        case IPlot::ValueType::OPEN:
            indicatorPlot->updatePlotAdd( static_cast<time_t>(it.key), it.open);
            break;

        case IPlot::ValueType::HIGH:
            indicatorPlot->updatePlotAdd( static_cast<time_t>(it.key), it.high);
            break;

        case IPlot::ValueType::LOW:
            indicatorPlot->updatePlotAdd( static_cast<time_t>(it.key), it.low);
            break;

        case IPlot::ValueType::CLOSE:
            indicatorPlot->updatePlotAdd( static_cast<time_t>(it.key), it.close);
            break;
        default:
            break;
        }
    }

    //add this indicator to our list
    activeIndicatorPlots_[indicatorType].push_back(std::move(indicatorPlot));
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

