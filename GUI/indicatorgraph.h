#ifndef INDICATORGRAPH_H
#define INDICATORGRAPH_H

#include <memory>
#include <array>
#include "qcustomplot.h"
#include "../BaseModules/Indicators/Indicators.h"

enum class IndicatorType
{
    CANDLEVOLUME,
    ABS,
    ACOS,
    AD,
    ADD,
    ADOSC,
    ADX,
    ADXR,
    AO,
    APO,
    AROON,
    AROONOSC,
    ASIN,
    ATAN,
    ATR,
    AVGPRICE,
    BBANDS,
    BOP,
    CCI,
    CEIL,
    CMO,
    COS,
    COSH,
    CROSSANY,
    CROSSOVER,
    CVI,
    DECAY,
    DEMA,
    DI,
    DIV,
    DM,
    DPO,
    DX,
    EDECAY,
    EMA,
    EMV,
    EXP,
    FISHER,
    FLOOR,
    FOSC,
    HMA,
    KAMA,
    KVO,
    LAG,
    LINREG,
    LINREGINTERCEPT,
    LINREGSLOPE,
    LN,
    LOG10,
    MACD,
    MARKETFI,
    MASS,
    MAX,
    MD,
    MEDPRICE,
    MFI,
    MIN,
    MOM,
    MSW,
    MUL,
    NATR,
    NVI,
    OBV,
    PPO,
    PSAR,
    PVI,
    QSTICK,
    ROC,
    ROCR,
    ROUND,
    RSI,
    SIN,
    SINH,
    SMA,
    SQRT,
    STDDEV,
    STDERR,
    STOCH,
    STOCHRSI,
    SUB,
    SUM,
    TAN,
    TANH,
    TEMA,
    TODEG,
    TORAD,
    TR,
    TRIMA,
    TRIX,
    TRUNC,
    TSF,
    TYPPRICE,
    ULTOSC,
    VAR,
    VHF,
    VIDYA,
    VOLATILITY,
    VOSC,
    VWMA,
    WAD,
    WCPRICE,
    WILDERS,
    WILLR,
    WMA,
    ZLEMA
};

enum class OhlcType
{
    OPEN,
    HIGH,
    LOW,
    CLOSE,
    VOLUME,
    VALUE
};

enum class IndicatorDisplayType
{
    SIMPLE,
    INDICATOR,
    OVERLAY,
    MATH
};

class IIndicatorGraph
{
public:
    IIndicatorGraph(){}
    virtual ~IIndicatorGraph(){}
    // interface for updating the plot
    virtual void updatePlotAdd(const time_t candleTime, double value) = 0;
    virtual void updatePlotReplace(const time_t candleTime, double value) = 0;
    virtual void rescaleValueAxisAutofit() = 0;
    virtual std::list<QCPAbstractPlottable*> getPlottables(void) = 0;

    // indicates which value this plot monitors.
    // o/h/l/c, volume, or just value.
    OhlcType valueType;
    IndicatorDisplayType displayType;
};

// Templatize for indicators
template <typename T>
class IndicatorGraph : public IIndicatorGraph
{
public:
    IndicatorGraph(QCPAxisRect& axisRect, std::unique_ptr<T> indicator, OhlcType type, IndicatorDisplayType display);
    ~IndicatorGraph() override;

    void updatePlotAdd(const time_t candleTime, double value) override;
    void updatePlotReplace(const time_t candleTime, double value) override;
    void rescaleValueAxisAutofit() override;
    std::list<QCPAbstractPlottable *> getPlottables(void) override;

private:
    QCPAxisRect& axisRect_;

    std::array<QCPGraph*, T::SIZE> graphs_; // one graph for each output from indicator class
    std::array<QSharedPointer<QCPDataContainer<QCPGraphData>>, T::SIZE> graphDataContainers_;
    std::unique_ptr<T> indicator_;
    int size_;

};

template <typename T>
IndicatorGraph<T>::IndicatorGraph(QCPAxisRect &axisRect, std::unique_ptr<T> indicator, OhlcType type, IndicatorDisplayType display):
    indicator_(std::move(indicator)),
    axisRect_(axisRect)
{
    valueType = type;
    displayType = display;
    size_ = 0;

    for(int i = 0; i < T::SIZE; ++i)
    {
        graphDataContainers_[i] = QSharedPointer<QCPDataContainer<QCPGraphData>>(new QCPDataContainer<QCPGraphData>);
        QCPAxis* keyAxis_ = axisRect_.axis(QCPAxis::atBottom);
        QCPAxis* valueAxis_ = nullptr;

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
IndicatorGraph<T>::~IndicatorGraph()
{
}

template <typename T>
void IndicatorGraph<T>::updatePlotAdd(const time_t candleTime, double value)
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
void IndicatorGraph<T>::updatePlotReplace(const time_t candleTime, double value)
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
void IndicatorGraph<T>::rescaleValueAxisAutofit()
{
    for(int i = 0; i < T::SIZE; ++i)
    {
        graphs_[i]->rescaleValueAxis(true, true);
    }
}

template<typename T>
std::list<QCPAbstractPlottable *> IndicatorGraph<T>::getPlottables()
{
    std::list<QCPAbstractPlottable *> plottables;
    for(int i = 0; i < T::SIZE; ++i)
    {
        plottables.push_back(graphs_[i]);
    }
    return plottables;
}
#endif // INDICATORGRAPH_H
