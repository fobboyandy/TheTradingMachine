#ifndef INDICATORGRAPH_H
#define INDICATORGRAPH_H

#include <memory>
#include "qcustomplot.h"
#include "../Indicators/Indicators/Indicator.h"
#include "iplot.h"

// Templatize for indicators
template <typename GraphType, typename IndicatorType>
class IndicatorPlot : public IPlot
{
public:
    IndicatorPlot(QCPAxisRect& axisRect, std::shared_ptr<IndicatorType> indicator);
    ~IndicatorPlot() override;

    void updatePlotNewCandle(const time_t candleTime, const Bar &candle);
    void updatePlotReplaceCandle(const time_t candleTime, const Bar &candle);

private:
    std::shared_ptr<IndicatorType> indicator_;
    // all plots must have an axis rect and a graph
    QCPAxisRect& axisRect_;
    GraphType* graph_;
};

template<typename GraphType, typename IndicatorType>
IndicatorPlot<GraphType, IndicatorType>::IndicatorPlot(QCPAxisRect &axisRect, std::shared_ptr<IndicatorType> indicator):
    indicator_(indicator),
    axisRect_(axisRect),
    graph_(new GraphType(axisRect_.axis(QCPAxis::atBottom), axisRect_.axis(QCPAxis::atLeft)))
{

}

template<typename GraphType, typename IndicatorType>
IndicatorPlot<GraphType, IndicatorType>::~IndicatorPlot()
{
    // simply delete. new can't return nullptr so graph_ can't be nullptr
    delete graph_;

    // maybe replot graph here?
}

template<typename GraphType, typename IndicatorType>
void IndicatorPlot<GraphType, IndicatorType>::updatePlotNewCandle(const time_t candleTime, const Bar &candle)
{
    auto indicatorDataPoints = indicator_->computeIndicator(DataPoint{candleTime, candle.close});
}

template<typename GraphType, typename IndicatorType>
void IndicatorPlot<GraphType, IndicatorType>::updatePlotReplaceCandle(const time_t candleTime, const Bar &candle)
{
    auto indicatorDataPoints = indicator_->recomputeIndicator(DataPoint{candleTime, candle.close});
}

#endif // INDICATORGRAPH_H
