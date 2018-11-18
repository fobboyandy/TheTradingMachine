#include "annotationplot.h"


AnnotationPlot::AnnotationPlot(QCustomPlot& t_parentPlot):
    parentPlot_(t_parentPlot)
{

}

AnnotationPlot::~AnnotationPlot()
{

}

void AnnotationPlot::addAnnotation(std::shared_ptr<Annotation::IAnnotation> t_annotation, QCPAxis& t_keyAxis, QCPAxis& t_valueAxis)
{
    switch(t_annotation->type())
    {
    case Annotation::AnnotationType::LINE:
    {
        auto lineAnnotation = std::dynamic_pointer_cast<Annotation::Line>(t_annotation);
        auto lineItem = new QCPItemLine(&parentPlot_);
        lineItem->start->setAxes(&t_keyAxis, &t_valueAxis);
        lineItem->start->setCoords(lineAnnotation->startX_, lineAnnotation->startY_);
        lineItem->end->setCoords(lineAnnotation->endX_, lineAnnotation->endY_);
    }
        break;
    case Annotation::AnnotationType::DOT:
        break;

    case Annotation::AnnotationType::CIRCLE:
    {

    }
        break;

    case Annotation::AnnotationType::LABEL:
        break;
    }
}

