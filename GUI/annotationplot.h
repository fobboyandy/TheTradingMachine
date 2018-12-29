#ifndef ANNOTATIONPLOT_H
#define ANNOTATIONPLOT_H

#include "baseplot.h"

class AnnotationPlot : public BasePlot
{
    Q_OBJECT
public:
    AnnotationPlot(QCustomPlot& t_parentPlot);
    ~AnnotationPlot() override;

    // override these functions to do nothing
    void updatePlotAdd(const Candlestick &candle) override;
    void updatePlotReplace(const Candlestick &candle) override;
    void pastCandlesPlotUpdate(std::shared_ptr<IIndicatorGraph> iplot) override;

    // this is called periodically from an external event
    // it must be implemented by the derived class to determine
    // how to rescale its own plot
    void rescalePlot() override;

private slots:

    virtual void xAxisChanged(QCPRange range) override;

private:
    void indicatorSelectionMenu(QPoint pos) override;
};

#endif // ANNOTATIONPLOT_H
