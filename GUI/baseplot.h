#ifndef IPLOT_H
#define IPLOT_H

#include "qcustomplot.h"

class BasePlot : public QObject
{
    Q_OBJECT
public:
    BasePlot(QCustomPlot& t_parentPlot);
    virtual ~BasePlot();

private:

private slots:
    virtual void xAxisChanged(QCPRange range);

protected:
    // This is not a QT object. We simply allocate the object
    // as a member instead of using pointers as in the QT model
    QCustomPlot& parentPlot_;
    QCPAxisRect axisRect_;

};

#endif // IPLOT_H
