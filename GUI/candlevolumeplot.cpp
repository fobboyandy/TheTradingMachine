#include "candlevolumeplot.h"

CandleVolumePlot::CandleVolumePlot(QCPAxisRect &candleAxisRect, QCPAxisRect &volumeAxisRect):
    candleAxisRect_(candleAxisRect),
    volumeAxisRect_(volumeAxisRect),
    candleBars_(new QCPFinancial(candleAxisRect.axis(QCPAxis::atBottom), candleAxisRect.axis(QCPAxis::atLeft))),
    volumeBars_(new QCPBars(volumeAxisRect.axis(QCPAxis::atBottom), volumeAxisRect.axis(QCPAxis::atLeft))),
    candleBarsDataContainer_(new QCPFinancialDataContainer),
    volumeBarsDataContainer_(new QCPBarsDataContainer),
    size_(0)
{
    candleBars_->setData(candleBarsDataContainer_);
    candleBars_->setWidthType(QCPFinancial::WidthType::wtPlotCoords);
    candleBars_->setWidth(60);

    volumeBars_->setData(volumeBarsDataContainer_);
    volumeBars_->setWidthType(QCPBars::WidthType::wtPlotCoords);
    volumeBars_->setWidth(candleBars_->width());

    // set volume bars color
    volumeBars_->setPen(Qt::NoPen);
    volumeBars_->setBrush(QColor(30, 144, 255));
}

CandleVolumePlot::~CandleVolumePlot()
{
    // parents (axisRects) clean up when they go out of scope
}

void CandleVolumePlot::updatePlotNewCandle(const time_t candleTime, const Bar &candle)
{
    // add a new bar volume and candlesticks
    candleBarsDataContainer_->add(QCPFinancialData(candleTime, candle.open, candle.high, candle.low, candle.close));
    volumeBarsDataContainer_->add(QCPBarsData(candleTime, candle.volume));
    ++size_;
}

void CandleVolumePlot::updatePlotReplaceCandle(const time_t candleTime, const Bar &candle)
{
    if(size_ > 0)
    {
        candleBarsDataContainer_->set(size_ - 1, QCPFinancialData(candleTime , candle.open, candle.high, candle.low, candle.close));
        volumeBarsDataContainer_->set(size_ - 1, QCPBarsData(candleTime, candle.volume));
    }
}

void CandleVolumePlot::rescaleValueAxisAutofit()
{
    candleBars_->rescaleValueAxis(false, true);
    volumeBars_->rescaleValueAxis(false, true);
}

double CandleVolumePlot::lowerRange()
{
    return candleBars_->data()->at(0)->mainKey();
}

double CandleVolumePlot::upperRange()
{
    return candleBars_->data()->at(candleBars_->data()->size() - 1)->mainKey();
}

int CandleVolumePlot::size()
{
    return size_;
}
