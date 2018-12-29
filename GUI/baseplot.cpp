#include "baseplot.h"
#include <QFont>

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
    // pointer is coming in from user. check for validity
    if(t_annotation == nullptr)
    {
        return;
    }

    QPen pen;
    pen.setColor(QColor(t_annotation->color_.red, t_annotation->color_.green, t_annotation->color_.blue));

    switch(t_annotation->type_)
    {
        case Annotation::AnnotationType::LINE:
        {
            auto lineItem = new QCPItemLine(&parentPlot_);
            auto lineAnnotation = std::dynamic_pointer_cast<Annotation::Line>(t_annotation);

            // set to this axis rect
            lineItem->setClipAxisRect(&axisRect_);
            lineItem->start->setAxisRect(&axisRect_);
            lineItem->end->setAxisRect(&axisRect_);
            lineItem->start->setAxes(axisRect_.axis(QCPAxis::AxisType::atBottom), axisRect_.axis(QCPAxis::AxisType::atLeft));
            lineItem->end->setAxes(axisRect_.axis(QCPAxis::AxisType::atBottom), axisRect_.axis(QCPAxis::AxisType::atLeft));

            // set coords
            lineItem->start->setCoords(lineAnnotation->startX_, lineAnnotation->startY_);
            lineItem->end->setCoords(lineAnnotation->endX_, lineAnnotation->endY_);

            lineItem->setPen(pen);

        }
            break;

        case Annotation::AnnotationType::LABEL:
        {
            auto textLabel = new QCPItemText(&parentPlot_);
            auto textAnnotation = std::dynamic_pointer_cast<Annotation::Label>(t_annotation);

            textLabel->setClipAxisRect(&axisRect_);
            textLabel->position->setAxisRect(&axisRect_);
            textLabel->position->setAxes(axisRect_.axis(QCPAxis::AxisType::atBottom), axisRect_.axis(QCPAxis::AxisType::atLeft));
            textLabel->position->setType(QCPItemPosition::PositionType::ptPlotCoords);
            textLabel->position->setCoords(textAnnotation->x_, textAnnotation->y_); // place position at center/top of axis rect

            QFont serifFont("Times", 5, QFont::Bold);
            textLabel->setText(QString(textAnnotation->text_.c_str()));
            textLabel->setPen(pen);

        }
            break;

        case Annotation::AnnotationType::CIRCLE:
        {
            auto circleItem = new QCPItemEllipse(&parentPlot_);
            auto circleAnnotation = std::dynamic_pointer_cast<Annotation::Circle>(t_annotation);

            // set to this axis rect
            circleItem->setClipAxisRect(&axisRect_);
            circleItem->topLeft->setAxisRect(&axisRect_);
            circleItem->bottomRight->setAxisRect(&axisRect_);
            circleItem->topLeft->setAxes(axisRect_.axis(QCPAxis::AxisType::atBottom), axisRect_.axis(QCPAxis::AxisType::atLeft));
            circleItem->bottomRight->setAxes(axisRect_.axis(QCPAxis::AxisType::atBottom), axisRect_.axis(QCPAxis::AxisType::atLeft));

            // set coords
            circleItem->topLeft->setCoords(circleAnnotation->x_ - circleAnnotation->radius_, circleAnnotation->y_ + circleAnnotation->radius_);
            circleItem->bottomRight->setCoords(circleAnnotation->x_ + circleAnnotation->radius_, circleAnnotation->y_ - circleAnnotation->radius_);

            circleItem->setPen(pen);
        }
            break;

        case Annotation::AnnotationType::DOT:
        {

        }
            break;

        case Annotation::AnnotationType::BOX:
        {
            auto boxItem = new QCPItemRect(&parentPlot_);
            const auto boxAnnotation = std::dynamic_pointer_cast<Annotation::Box>(t_annotation);

            auto upperLeftX = boxAnnotation->upperLeftX_;
            auto upperLeftY = boxAnnotation->upperLeftY_;

            auto lowerRightX = boxAnnotation->lowerRightX_;
            auto lowerRightY = boxAnnotation->lowerRightY_;

            // set to this axis rect
            boxItem->setClipAxisRect(&axisRect_);
            boxItem->topLeft->setAxisRect(&axisRect_);
            boxItem->bottomRight->setAxisRect(&axisRect_);
            boxItem->topLeft->setAxes(axisRect_.axis(QCPAxis::AxisType::atBottom), axisRect_.axis(QCPAxis::AxisType::atLeft));
            boxItem->bottomRight->setAxes(axisRect_.axis(QCPAxis::AxisType::atBottom), axisRect_.axis(QCPAxis::AxisType::atLeft));

            // set coords
            boxItem->topLeft->setCoords(upperLeftX, upperLeftY);
            boxItem->bottomRight->setCoords(lowerRightX, lowerRightY);

            boxItem->setPen(pen);
        }
            break;

    }

}

bool BasePlot::inRect(QPoint pos)
{
    auto xLowerBound = axisRect_.topLeft().x();
    auto xUpperBound = axisRect_.topRight().x();
    auto yLowerBound = axisRect_.topLeft().y();
    auto yUpperBound = axisRect_.bottomLeft().y();

    return pos.x() >= xLowerBound && pos.x() <= xUpperBound && pos.y() >= yLowerBound && pos.y() <= yUpperBound;
}

void BasePlot::removeIndicatorMenu(QPoint pos, QList<QCPAbstractPlottable *> plottables)
{
    QMenu* menu = new QMenu();
    // destroy the menu after closing
    menu->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);

    menu->addAction("Remove Indicator", this, [this, plottables]()
    {
        // get the value axis shared by these plots. check the number
        // of remaining plottables after removing them
        auto commonValueAxis = plottables.front()->valueAxis();

        // all plottables associated with a chosen iplot
        // are provided by the argument
        for(auto& plottable: plottables)
        {
            // erase the plot entries
            activeIndicatorPlots_.erase(plottable);

            // remove it from the graph
            parentPlot_.removePlottable(plottable);

        }

        // if there are no more plottables associated with
        // this axis, remove the axis
        if(commonValueAxis->plottables().size() == 0)
        {
            axisRect_.removeAxis(commonValueAxis);
        }
    });

    menu->popup(parentPlot_.mapToGlobal(pos));
}

void BasePlot::menuShow(QPoint pos)
{
    auto selectedPlottables = parentPlot_.selectedPlottables();
    // if no plottables were selected then show indicator selection
    if(selectedPlottables.size() == 0)
    {
        indicatorSelectionMenu(pos);
    }
    else
    {
        removeIndicatorMenu(pos, selectedPlottables);
    }
}

void BasePlot::plotSelectSlot(bool selected)
{
    // mark the other graphs as selected as well
    if(selected)
    {
        std::shared_ptr<IIndicatorGraph> selectedPlot;

        // find the graph that is currently selected
        for(auto& plottableEntry: activeIndicatorPlots_)
        {
            if(plottableEntry.first->selected())
            {
                selectedPlot = plottableEntry.second;
                break; // we know only one will be selected
            }
        }

        auto plottables = selectedPlot->getPlottables();
        for(auto& plottable: plottables)
        {
            // cast to qcpgraph since we only have qcpgraphs at the moment
            plottable->setSelection(QCPDataSelection(static_cast<QCPGraph*>(plottable)->data()->dataRange()));
        }
    }
}
