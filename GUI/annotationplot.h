#ifndef ANNOTATIONPLOT_H
#define ANNOTATIONPLOT_H


#include "../BaseAlgorithm/BaseAlgorithm/Annotation.h"
#include "qcustomplot.h"
#include <unordered_map>

class AnnotationPlot
{
public:
    AnnotationPlot(QCPAxis& t_keyAxis, QCPAxis& t_valueAxis);
    ~AnnotationPlot();

    void addAnnotation(std::shared_ptr<Annotation::IAnnotation> t_annotation);

private:
    QCPAxis& keyAxis_;
    QCPAxis& valueAxis_;

};

#endif // ANNOTATIONPLOT_H
