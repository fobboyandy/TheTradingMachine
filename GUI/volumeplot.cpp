#include "volumeplot.h"

VolumePlot::VolumePlot(QCPAxisRect &axisRect):
    axisRect_(axisRect),
    volumeBars_(new QCPBars(axisRect.axis(QCPAxis::atBottom), axisRect.axis(QCPAxis::atLeft))),
    dataContainer_(new QCPBarsDataContainer),
    size_(0)
{
    volumeBars_->setData(dataContainer_);
    volumeBars_->setWidthType(QCPBars::WidthType::wtPlotCoords);
    volumeBars_->setWidth(60);

    // set volume bars color
    volumeBars_->setPen(Qt::NoPen);
    volumeBars_->setBrush(QColor(30, 144, 255));
}

VolumePlot::~VolumePlot()
{

}

void VolumePlot::updatePlotAdd(const time_t candleTime, const Bar &candle)
{
    // add a new bar volume and candlesticks
    dataContainer_->add(QCPBarsData(candleTime, candle.volume));
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
        dataContainer_->set(size_ - 1, QCPBarsData(candleTime, candle.volume));

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

void VolumePlot::addIndicator(IPlot::IndicatorType indicatorType, std::unique_ptr<IPlot> indicatorPlot)
{
    // keep the indicatorPlot up to date with all the candles we currently have
    for(auto& it: *dataContainer_)
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

