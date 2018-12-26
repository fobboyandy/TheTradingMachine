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

    // since activeIndicatorPlots_ entries map plottables to
    // iplots, we can have more than one plottable mapped to the same
    // iplot. for example, macd has 3 values which can be mapped
    // to the same indicator. we use this unordered_set to mark
    // which has been updated as we traverse our entries
    std::unordered_set<std::shared_ptr<IIndicatorGraph>> updatedIndicators;

    // update all the indicators
    for(auto& activePlotIt: activeIndicatorPlots_)
    {
        if(updatedIndicators.find(activePlotIt.second) == updatedIndicators.end())
        {
            activePlotIt.second->updatePlotAdd(candle.time, candle.volume);
            // mark as updated
            updatedIndicators.insert(activePlotIt.second);
        }
    }
}

void VolumePlot::updatePlotReplace(const Candlestick &candle)
{
    if(size_ > 0)
    {
        volumeBars_->data()->set(size_ - 1, QCPBarsData(candle.time, candle.volume));

        std::unordered_set<std::shared_ptr<IIndicatorGraph>> updatedIndicators;
        // update all the indicators belonging to this plot
        for(auto& activePlotIt: activeIndicatorPlots_)
        {
            if(updatedIndicators.find(activePlotIt.second) == updatedIndicators.end())
            {
                activePlotIt.second->updatePlotReplace( candle.time, candle.volume);
                // mark as updated
                updatedIndicators.insert(activePlotIt.second);
            }
        }
    }
    // if there is nothing to replace, add a new one
    else
    {
        updatePlotAdd(candle);
    }
}

void VolumePlot::rescalePlot()
{
    volumeBars_->rescaleValueAxis(false, true);
}

void VolumePlot::pastCandlesPlotUpdate(std::shared_ptr<IIndicatorGraph> iplot)
{
    for(auto& it: *volumeBars_->data())
    {
        iplot->updatePlotAdd( static_cast<time_t>(it.key), it.value);
    }
}

void VolumePlot::addIndicator(IndicatorType indicatorType, std::unique_ptr<IIndicatorGraph> indicatorPlot)
{
    // keep the indicatorPlot up to date with all the candles we currently have
    for(auto& it: *volumeBars_->data())
    {
        indicatorPlot->updatePlotAdd( static_cast<time_t>(it.key), it.value);
    }
//    activeIndicatorPlots_[indicatorType].push_back(std::move(indicatorPlot));
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


void VolumePlot::indicatorSelectionMenu(QPoint pos)
{
    QMenu* menu = new QMenu();
    // destroy the menu after closing
    menu->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);

    // dynamically create a new graph each time based on the state of the plots
    // and indicators
    auto subMenu = menu->addMenu("A");

    // dynamically create a new graph each time based on the state of the plots
    // and indicators
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
            indicatorLaunch<SimpleMovingAverage>(OhlcType::VOLUME, IndicatorDisplayType::OVERLAY, period);
        }
    });

    menu->popup(parentPlot_.mapToGlobal(pos));
}

template<typename IndicatorType, typename ... Args>
void VolumePlot::indicatorLaunch(OhlcType valueType, IndicatorDisplayType displayType, Args... args)
{
    auto indicator = std::make_unique<IndicatorType>(args...);
    auto plot = std::make_shared<IndicatorGraph<IndicatorType>>(axisRect_, std::move(indicator), valueType, displayType);
    pastCandlesPlotUpdate(plot);
    auto plottables = plot->getPlottables();

    // add these plottables to our iplot map
    for(auto& plottable: plottables)
    {
        activeIndicatorPlots_[plottable] = plot;
        connect(plottable, SIGNAL(selectionChanged(bool)), this, SLOT(plotSelectSlot(bool)));
    }

    //replot after adding indicator
    parentPlot_.replot();
}
