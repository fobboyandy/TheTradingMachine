#include "annotationplot.h"

AnnotationPlot::AnnotationPlot(QCustomPlot& t_parentPlot) :
    BasePlot (t_parentPlot)
{
    // since annotation plots don't have auto scaling, we
    // allow users to manually adjust the axes
    axisRect_.setRangeDrag(Qt::Horizontal | Qt::Vertical);
    axisRect_.setRangeZoom(Qt::Horizontal);
}

AnnotationPlot::~AnnotationPlot()
{

}

void AnnotationPlot::updatePlotAdd(const Candlestick &candle)
{

}


void AnnotationPlot::updatePlotReplace(const Candlestick &candle)
{

}

void AnnotationPlot::pastCandlesPlotUpdate(std::shared_ptr<IIndicatorGraph> iplot)
{

}

void AnnotationPlot::rescalePlot()
{

}

void AnnotationPlot::xAxisChanged(QCPRange range)
{

}

