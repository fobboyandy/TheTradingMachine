#include "volumeplot.h"

VolumePlot::VolumePlot(QCustomPlot& t_parentPlot):
    BasePlot(t_parentPlot)
{
    volumeBars_ = new QCPBars(axisRect_.axis(QCPAxis::atBottom), axisRect_.axis(QCPAxis::atLeft));
    volumeBars_->setWidthType(QCPBars::WidthType::wtPlotCoords);
    volumeBars_->setWidth(60);

    // set volume bars color
    volumeBars_->setPen(Qt::NoPen);
    volumeBars_->setBrush(QColor(30, 144, 255));

    size_ = 0;
}

VolumePlot::~VolumePlot()
{

}

void VolumePlot::updatePlotAdd(const time_t candleTime, const Bar &candle)
{
    // add a new bar volume and candlesticks
    volumeBars_->addData(candleTime, candle.volume);
    ++size_;

    // recursively update all the indicators belonging to this plot
    for(auto& activePlotIt: activeIndicatorPlots_)
    {
        for(auto& plotIt: activePlotIt.second)
        {
            plotIt->updatePlotAdd(candleTime, candle.volume);
        }
    }
}

void VolumePlot::updatePlotReplace(const time_t candleTime, const Bar &candle)
{
    if(size_ > 0)
    {
        volumeBars_->data()->set(size_ - 1, QCPBarsData(candleTime, candle.volume));

        // recursively update all the indicators belonging to this plot
        for(auto& activePlotIt: activeIndicatorPlots_)
        {
            for(auto& plotIt: activePlotIt.second)
            {
                plotIt->updatePlotReplace(candleTime, candle.volume);
            }
        }
    }
}

void VolumePlot::rescaleValueAxisAutofit()
{
    volumeBars_->rescaleValueAxis(false, true);
}

void VolumePlot::addIndicator(IndicatorType indicatorType, std::unique_ptr<IIndicatorPlot> indicatorPlot)
{
    // keep the indicatorPlot up to date with all the candles we currently have
    for(auto& it: *volumeBars_->data())
    {
        indicatorPlot->updatePlotAdd( static_cast<time_t>(it.key), it.value);
    }
    activeIndicatorPlots_[indicatorType].push_back(std::move(indicatorPlot));
}

double VolumePlot::lowerRange()
{
    return volumeBars_->data()->at(0)->mainKey();
}

double VolumePlot::upperRange()
{
    return volumeBars_->data()->at(volumeBars_->data()->size() - 1)->mainKey();
}

int VolumePlot::size()
{
    return size_;
}

