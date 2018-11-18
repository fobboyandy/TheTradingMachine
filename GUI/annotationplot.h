#ifndef ANNOTATIONPLOT_H
#define ANNOTATIONPLOT_H


#include "../BaseAlgorithm/BaseAlgorithm/Annotation.h"
#include "qcustomplot.h"
#include <unordered_map>

// Manages the annotation of the current session. The associated axis of an
// annotation must be provided in addAnnotation. This allows the function
// to know where to paint the annotation.
class AnnotationPlot
{
public:
    AnnotationPlot(QCustomPlot& t_parentPlot);
    ~AnnotationPlot();

    void addAnnotation(std::shared_ptr<Annotation::IAnnotation> t_annotation, QCPAxis& t_keyAxis, QCPAxis& t_valueAxis);

private:
    QCustomPlot& parentPlot_;

};

#endif // ANNOTATIONPLOT_H
