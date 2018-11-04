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

    // recursively update all the indicators belonging to this plot
    for(auto& activePlotIt: activeIndicatorPlots_)
    {
        for(auto& plotIt: activePlotIt.second)
        {
            plotIt->updatePlotAdd(candleTime, candle.close);
        }
    }
}

void CandlePlot::updatePlotReplace(const time_t candleTime, const Bar &candle)
{
    if(size_ > 0)
    {
        dataContainer_->set(size_ - 1, QCPFinancialData(candleTime , candle.open, candle.high, candle.low, candle.close));
        // recursively update all the indicators belonging to this plot
        for(auto& activePlotIt: activeIndicatorPlots_)
        {
            for(auto& plotIt: activePlotIt.second)
            {
                plotIt->updatePlotReplace(candleTime, candle.close);
            }
        }
    }
}

void CandlePlot::rescaleValueAxisAutofit()
{
    candleBars_->rescaleValueAxis(false, true);
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

