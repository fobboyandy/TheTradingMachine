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
    IndicatorPlot(QCPAxisRect& axisRect, std::unique_ptr<T> indicator, OhlcType type, IndicatorDisplayType display);
    ~IndicatorPlot() override;

    void updatePlotAdd(const time_t candleTime, double value) override;
    void updatePlotReplace(const time_t candleTime, double value) override;
    void rescaleValueAxisAutofit() override;
    std::list<QCPAbstractPlottable *> getPlottables(void) override;

private:
    QCPAxisRect& axisRect_;
    QCPAxis* keyAxis_;
    QCPAxis* valueAxis_;

    std::array<QCPGraph*, T::SIZE> graphs_; // one graph for each output from indicator class
    std::array<QSharedPointer<QCPDataContainer<QCPGraphData>>, T::SIZE> graphDataContainers_;
    std::unique_ptr<T> indicator_;
    int size_;
};

template <typename T>
IndicatorPlot<T>::IndicatorPlot(QCPAxisRect &axisRect, std::unique_ptr<T> indicator, OhlcType type, IndicatorDisplayType display):
    indicator_(std::move(indicator)),
    axisRect_(axisRect)
{
    valueType = type;
    displayType = display;
    size_ = 0;

    for(int i = 0; i < T::SIZE; ++i)
    {
        graphDataContainers_[i] = QSharedPointer<QCPDataContainer<QCPGraphData>>(new QCPDataContainer<QCPGraphData>);
        keyAxis_ = axisRect_.axis(QCPAxis::atBottom);

        switch(display)
        {
            // for indicators, we make a new axis
            // so that we can share the same display
            case IndicatorDisplayType::INDICATOR:
                valueAxis_ = axisRect.addAxis(QCPAxis::atLeft);
                break;

            case IndicatorDisplayType::MATH:
                break;

            // for overlays, we share the same axis as the rect
            case IndicatorDisplayType::OVERLAY:
                valueAxis_ = axisRect_.axis(QCPAxis::atLeft);
                break;

            case IndicatorDisplayType::SIMPLE:
                break;
        }
        graphs_[i] = new QCPGraph(keyAxis_, valueAxis_);
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
    auto indicatorDataPoints = indicator_->computeIndicatorPoint(value);

    // indicatorDataPoints return the same number of points as
    // our number of graphs in graphs_. Plot these points
    // as separate graphs
    for(int i = 0; i < T::SIZE; ++i)
    {
        graphDataContainers_[i]->add(QCPGraphData(candleTime, indicatorDataPoints[i]));
    }
    ++size_;
}

template <typename T>
void IndicatorPlot<T>::updatePlotReplace(const time_t candleTime, double value)
{
	if (size_ > 0)
	{
		auto indicatorDataPoints = indicator_->recomputeIndicatorPoint(value);
		for (int i = 0; i < T::SIZE; ++i)
		{
			// replace the previous point with the updated value;
			graphDataContainers_[i]->set(size_ - 1, QCPGraphData(candleTime, indicatorDataPoints[i]));
		}
	}
}

template <typename T>
void IndicatorPlot<T>::rescaleValueAxisAutofit()
{
    for(int i = 0; i < T::SIZE; ++i)
    {
        graphs_[i]->rescaleValueAxis(true, true);
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
