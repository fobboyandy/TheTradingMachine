#ifndef IPLOT_H
#define IPLOT_H

#include "qcustomplot.h"
#include "indicatorplot.h"
#include "../InteractiveBrokersClient/InteractiveBrokersApi/bar.h"

class BasePlot : public QObject
{
    Q_OBJECT
public:
    BasePlot(QCustomPlot& t_parentPlot);
    virtual ~BasePlot();

    virtual void updatePlotAdd(const time_t candleTime, const Bar &candle) = 0;
    virtual void updatePlotReplace(const time_t candleTime, const Bar &candle) = 0;
    virtual void pastCandlesPlotUpdate(std::shared_ptr<IIndicatorPlot> iplot) = 0;
    virtual void rescalePlot() = 0;

private:

private slots:
    // there should be a way to define this in the base class instead of
    // letting the derived class define this function since the functionality is
    // generic for all derived classes. for now, we will use pure virtual
    virtual void xAxisChanged(QCPRange range) = 0;

protected:
    // This is not a QT object. We simply allocate the object
    // as a member instead of using pointers as in the QT model
    QCustomPlot& parentPlot_;
    QCPAxisRect axisRect_;

    bool autoScaleKeyAxis_;

};

#endif // IPLOT_H
