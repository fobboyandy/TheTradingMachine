#ifndef INDICATORGRAPH_H
#define INDICATORGRAPH_H

#include <memory>
#include <array>
#include "qcustomplot.h"
#include "../Indicators/Indicators/Common.h"
#include "iplot.h"

// Templatize for indicators
template <typename IndicatorType>
class IndicatorPlot : public IPlot
{
public:
    IndicatorPlot(QCPAxisRect& axisRect, std::unique_ptr<IndicatorType> indicator);
    ~IndicatorPlot() override;

    void updatePlotNewCandle(const time_t candleTime, const Bar &candle) override;
    void updatePlotReplaceCandle(const time_t candleTime, const Bar &candle) override;
    void rescaleValueAxisAutofit() override;

private:

    //use polymorphism to handle various indicators
    std::unique_ptr<IndicatorType> indicator_;
    // all plots must have an axis rect and a graph
    QCPAxisRect& axisRect_;

    //
    // one graph for each output from indicator class
    //
    std::array<std::unique_ptr<QCPGraph>, IndicatorType::SIZE> graphs_;
    std::array<QSharedPointer<QCPDataContainer<QCPGraphData>>, IndicatorType::SIZE> graphDataContainers_;
};

template <typename IndicatorType>
IndicatorPlot<IndicatorType>::IndicatorPlot(QCPAxisRect &axisRect, std::unique_ptr<IndicatorType> indicator):
    indicator_(std::move(indicator)),
    axisRect_(axisRect)
{
    for(int i = 0; i < IndicatorType::SIZE; ++i)
    {
        graphs_[i] = std::make_unique<QCPGraph>(axisRect_.axis(QCPAxis::atBottom), axisRect_.axis(QCPAxis::atLeft));
        graphs_[i]->setData(graphDataContainers_[i]);
    }
}

template <typename IndicatorType>
IndicatorPlot<IndicatorType>::~IndicatorPlot()
{
    // simply delete. new can't return nullptr so graph_ can't be nullptr
}

template <typename IndicatorType>
void IndicatorPlot<IndicatorType>::updatePlotNewCandle(const time_t candleTime, const Bar &candle)
{
    auto indicatorDataPoints = indicator_->computeIndicatorPoint(DataPoint{candleTime, candle.close});
}

template <typename IndicatorType>
void IndicatorPlot<IndicatorType>::updatePlotReplaceCandle(const time_t candleTime, const Bar &candle)
{
    for(int i = 0; i < IndicatorType::SIZE; ++i)
    {
        if (graphDataContainers_[i]->size() > 0)
        {
            auto indicatorDataPoints = indicator_->recomputeIndicatorPoint(DataPoint{candleTime, candle.close});
        }

    }
}

template <typename IndicatorType>
void IndicatorPlot<IndicatorType>::rescaleValueAxisAutofit()
{

}

#endif // INDICATORGRAPH_H
