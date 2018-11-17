#include "annotationplot.h"


AnnotationPlot::AnnotationPlot(QCPAxis &t_keyAxis, QCPAxis &t_valueAxis):
    keyAxis_(t_keyAxis),
    valueAxis_(t_valueAxis)
{

}

AnnotationPlot::~AnnotationPlot()
{

}

void AnnotationPlot::addAnnotation(std::shared_ptr<Annotation::IAnnotation> t_annotation)
{
    switch(t_annotation->type())
    {
    case Annotation::AnnotationType::LINE:
    {
        auto line = std::dynamic_pointer_cast<Annotation::Line>(t_annotation);
        auto annotationPlottable = new QCPGraph(&keyAxis_, &valueAxis_);
        annotationPlottable->setLineStyle(QCPGraph::LineStyle::lsLine);
        annotationPlottable->addData(QVector<double>{line->startX_, line->endX_}, QVector<double>{line->startY_, line->endY_});
    }
        break;
    case Annotation::AnnotationType::DOT:
        break;

    case Annotation::AnnotationType::CIRCLE:
        break;

    case Annotation::AnnotationType::LABEL:
        break;
    }
}

