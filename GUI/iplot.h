#ifndef IPLOT_H
#define IPLOT_H

// base class for all plots
// interface class for storing base class ptrs for various
// indicator plots in a container

#include "../InteractiveBrokersClient/InteractiveBrokersApi/bar.h"
#include "qcustomplot.h"
#include <list>
#include <ctime>
// define IPlotIndex
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

class IPlot
{
public:
    IPlot(){}
    virtual ~IPlot(){}
    // interface for updating the plot
    virtual void updatePlotAdd(const time_t candleTime, double value) = 0;
    virtual void updatePlotReplace(const time_t candleTime, double value) = 0;
    virtual void rescaleValueAxisAutofit() = 0;
    virtual std::list<QCPAbstractPlottable*> getPlottables(void) = 0;

    // indicates which value this plot monitors.
    // o/h/l/c, volume, or just value.
    OhlcType valueType;
};



#endif // IPLOT_H


