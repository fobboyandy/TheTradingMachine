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

void VolumePlot::updatePlotAdd(const Candlestick &candle)
{
    // add a new bar volume and candlesticks
    volumeBars_->addData(candle.time, candle.volume);
    ++size_;

    // recursively update all the indicators belonging to this plot
    for(auto& activePlotIt: activeIndicatorPlots_)
    {
        for(auto& plotIt: activePlotIt.second)
        {
            plotIt->updatePlotAdd(candle.time, candle.volume);
        }
    }
}

void VolumePlot::updatePlotReplace(const Candlestick &candle)
{
    if(size_ > 0)
    {
        volumeBars_->data()->set(size_ - 1, QCPBarsData(candle.time, candle.volume));

        // recursively update all the indicators belonging to this plot
        for(auto& activePlotIt: activeIndicatorPlots_)
        {
            for(auto& plotIt: activePlotIt.second)
            {
                plotIt->updatePlotReplace(candle.time, candle.volume);
            }
        }
    }
}

void VolumePlot::rescalePlot()
{
    volumeBars_->rescaleValueAxis(false, true);
}

void VolumePlot::pastCandlesPlotUpdate(std::shared_ptr<IIndicatorPlot> iplot)
{

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

void VolumePlot::xAxisChanged(QCPRange range)
{
    // compare the range of our zoom with our data.
    // if the zoom contains all the data, then autoscale
    if(floor(range.lower) <= volumeBars_->data()->at(0)->mainKey() &&
        ceil(range.upper) >= volumeBars_->data()->at(volumeBars_->data()->size() - 1)->mainKey())
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
