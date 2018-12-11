#ifndef IPLOT_H
#define IPLOT_H

#include "qcustomplot.h"
#include "indicatorplot.h"
#include "../BaseModules/CandleMaker/CandleMaker.h"
#include "../BaseModules/BaseAlgorithm/Annotation.h"

class BasePlot : public QObject
{
    Q_OBJECT
public:
    BasePlot(QCustomPlot& t_parentPlot);
    virtual ~BasePlot();

    virtual void updatePlotAdd(const Candlestick &candle) = 0;
    virtual void updatePlotReplace(const Candlestick &candle) = 0;
    virtual void pastCandlesPlotUpdate(std::shared_ptr<IIndicatorPlot> iplot) = 0;

    // this is called periodically from an external event
    // it must be implemented by the derived class to determine
    // how to rescale its own plot
    virtual void rescalePlot() = 0;

    // simply add the annotation to the default axis for now. later implementation will
    // add the annotation to a chosen axis
    void addAnnotation(std::shared_ptr<Annotation::IAnnotation> t_annotation);

private:

private slots:
    // there should be a way to define this in the base class instead of
    // letting the derived class define this function since the functionality is
    // generic for all derived classes. for now, we will use pure virtual
    virtual void xAxisChanged(QCPRange range) = 0;

protected:

    // keeping a reference to the parentPlot because this object is not responsible
    // of cleaning up dynamically allocated objects
    QCustomPlot& parentPlot_;
    QCPAxisRect axisRect_;

    bool autoScaleKeyAxis_;

};

#endif // IPLOT_H
