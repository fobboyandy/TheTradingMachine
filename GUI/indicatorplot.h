#ifndef INDICATORGRAPH_H
#define INDICATORGRAPH_H

#include <memory>
#include <array>
#include "qcustomplot.h"
#include "../Indicators/Indicators/Common.h"
#include "iplot.h"
#include <iostream>

// Templatize for indicators
template <typename T>
class IndicatorPlot : public IPlot
{
public:
    IndicatorPlot(QCPAxisRect& axisRect, std::unique_ptr<T> indicator, IPlot::ValueType valueType);
    ~IndicatorPlot() override;

    void updatePlotAdd(const time_t candleTime, double value) override;
    void updatePlotReplace(const time_t candleTime, double value) override;
    void rescaleValueAxisAutofit() override;
    std::list<QCPAbstractPlottable *> getPlottables(void) override;

private:
    QCPAxisRect& axisRect_;
    std::array<QCPGraph*, T::SIZE> graphs_; // one graph for each output from indicator class
    std::array<QSharedPointer<QCPDataContainer<QCPGraphData>>, T::SIZE> graphDataContainers_;
    std::unique_ptr<T> indicator_;

};

template <typename T>
IndicatorPlot<T>::IndicatorPlot(QCPAxisRect &axisRect, std::unique_ptr<T> indicator, IPlot::ValueType type):
    indicator_(std::move(indicator)),
    axisRect_(axisRect)
{
    valueType = type;
    for(int i = 0; i < T::SIZE; ++i)
    {
        graphDataContainers_[i] = QSharedPointer<QCPDataContainer<QCPGraphData>>(new QCPDataContainer<QCPGraphData>);
        graphs_[i] = new QCPGraph(axisRect_.axis(QCPAxis::atBottom), axisRect_.axis(QCPAxis::atLeft));
        graphs_[i]->setData(graphDataContainers_[i]);
    }
}

template <typename T>
IndicatorPlot<T>::~IndicatorPlot()
{
}

template <typename T>
void IndicatorPlot<T>::updatePlotAdd(const time_t candleTime, double value)
{
    auto indicatorDataPoints = indicator_->computeIndicatorPoint(DataPoint{candleTime, value});

    // indicatorDataPoints return the same number of points as
    // our number of graphs in graphs_. Plot these points
    // as separate graphs
    for(int i = 0; i < T::SIZE; ++i)
    {
        graphDataContainers_[i]->add(QCPGraphData(indicatorDataPoints[i].time, indicatorDataPoints[i].value));
    }
}

template <typename T>
void IndicatorPlot<T>::updatePlotReplace(const time_t candleTime, double value)
{
    for(int i = 0; i < T::SIZE; ++i)
    {
        if (graphDataContainers_[i]->size() > 0)
        {
            auto indicatorDataPoints = indicator_->recomputeIndicatorPoint(DataPoint{candleTime, value});

            // replace the previous point with the updated value;
            graphDataContainers_[i]->set(graphDataContainers_[i]->size() - 1, QCPGraphData(indicatorDataPoints[i].time, indicatorDataPoints[i].value));
        }
    }
}

template <typename T>
void IndicatorPlot<T>::rescaleValueAxisAutofit()
{
    for(int i = 0; i < T::SIZE; ++i)
    {
        graphs_[i]->rescaleValueAxis(false, true);
    }
}

template<typename T>
std::list<QCPAbstractPlottable *> IndicatorPlot<T>::getPlottables()
{
    std::list<QCPAbstractPlottable *> plottables;
    for(int i = 0; i < T::SIZE; ++i)
    {
        plottables.push_back(graphs_[i]);
    }
    return plottables;
}

#endif // INDICATORGRAPH_H
