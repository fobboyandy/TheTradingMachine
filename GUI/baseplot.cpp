#include "baseplot.h"

BasePlot::BasePlot(QCustomPlot &t_parentPlot):
    QObject(&t_parentPlot),
    parentPlot_(t_parentPlot),
    axisRect_(&t_parentPlot)
{
    axisRect_.setRangeDrag(Qt::Horizontal);
    axisRect_.setRangeZoom(Qt::Horizontal);

    // create a time axis. we don't plan on modifying
    // the time axis so we dont need to keep the handle.
    // once axis rect has ownership, it will do the clean up
    auto xTimeAxis = QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime);
    xTimeAxis->setDateTimeFormat("hh:mm:ss");
    axisRect_.axis(QCPAxis::atBottom)->setTicker(xTimeAxis);
    parentPlot_.plotLayout()->addElement(&axisRect_);

    // make left side margins line up with existing ones margins
    // if this is the first axis rect, create new margin group.
    // axisRect() can't be null since we performed addElement above.
    // we don't need to index into a particular axisRect if all
    // axis rects are aligned the same upon construction
    auto leftMarginGroup = parentPlot_.axisRect()->marginGroup(QCP::MarginSide::msLeft);
    if(leftMarginGroup == nullptr)
    {
        leftMarginGroup = new QCPMarginGroup(&parentPlot_);
    }
    axisRect_.setMarginGroup(QCP::msLeft, leftMarginGroup);

    auto rightMarginGroup = parentPlot_.axisRect()->marginGroup(QCP::MarginSide::msRight);
    // do the same for the right side
    if(rightMarginGroup == nullptr)
    {
        rightMarginGroup = new QCPMarginGroup(&parentPlot_);
    }
    axisRect_.setMarginGroup(QCP::msRight, rightMarginGroup);

    // make all rects scroll together in xAxis
    // by connecting this axisRect's xAxis signal to
    // the existing rects
    auto allRects = parentPlot_.axisRects();
    for(auto rect: allRects)
    {
        // skip self rect
        if(rect != &axisRect_)
        {
            QObject::connect(axisRect_.axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), rect->axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
            QObject::connect(rect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), axisRect_.axis(QCPAxis::atBottom), SLOT(setRange(QCPRange)));
        }
    }

    connect(axisRect_.axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    autoScaleKeyAxis_ = true;
}

BasePlot::~BasePlot()
{

}

void BasePlot::addAnnotation(std::shared_ptr<Annotation::IAnnotation> t_annotation)
{
    switch(t_annotation->type())
    {
    case Annotation::AnnotationType::LINE:
    {
        auto newLineItem = new QCPItemLine(&parentPlot_);
        auto lineAnnotation = std::dynamic_pointer_cast<Annotation::Line>(t_annotation);

        // set to this axis rect
        newLineItem->setClipToAxisRect(false);
        newLineItem->start->setAxisRect(&axisRect_);
        newLineItem->end->setAxisRect(&axisRect_);

        // set coords
        newLineItem->start->setCoords(lineAnnotation->startX_, lineAnnotation->startY_);
        newLineItem->end->setCoords(lineAnnotation->endX_, lineAnnotation->endY_);
    }
        break;

    case Annotation::AnnotationType::LABEL:
    {

    }
        break;

    case Annotation::AnnotationType::CIRCLE:
    {

    }
        break;

    case Annotation::AnnotationType::DOT:
    {

    }
        break;

    }

}
