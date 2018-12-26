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
    std::cout << "annotation plot" << std::endl;
}

void AnnotationPlot::rescalePlot()
{

}

void AnnotationPlot::xAxisChanged(QCPRange range)
{

}


void AnnotationPlot::indicatorSelectionMenu(QPoint pos)
{
    QMenu* menu = new QMenu();
    // destroy the menu after closing
    menu->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);

    // dynamically create a new graph each time based on the state of the plots
    // and indicators
    auto subMenu = menu->addMenu("A");
    subMenu->addAction("Annotation");

    menu->popup(parentPlot_.mapToGlobal(pos));
}
