#ifndef IPLOT_H
#define IPLOT_H

#include "qcustomplot.h"
#include "indicatorgraph.h"
#include "indicatordialog.h"
#include "indicatorincludes.h"
#include "../BaseModules/CandleMaker/CandleMaker.h"
#include "../BaseModules/BaseAlgorithm/Annotation.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iostream>
#include <string>

class BasePlot : public QObject
{
    Q_OBJECT
public:
    BasePlot(QCustomPlot& t_parentPlot);
    virtual ~BasePlot();

    virtual void updatePlotAdd(const Candlestick &candle) = 0;
    virtual void updatePlotReplace(const Candlestick &candle) = 0;
    virtual void pastCandlesPlotUpdate(std::shared_ptr<IIndicatorGraph> iplot) = 0;

    // this is called periodically from an external event
    // it must be implemented by the derived class to determine
    // how to rescale its own plot
    virtual void rescalePlot() = 0;

    // simply add the annotation to the default axis for now. later implementation will
    // add the annotation to a chosen axis
    void addAnnotation(std::shared_ptr<Annotation::IAnnotation> t_annotation);

    void menuShow(QPoint pos);
    bool inRect(QPoint pos);
private:
    virtual void indicatorSelectionMenu(QPoint pos) = 0;

private slots:
    // there should be a way to define this in the base class instead of
    // letting the derived class define this function since the functionality is
    // generic for all derived classes. for now, we will use pure virtual
    virtual void xAxisChanged(QCPRange range) = 0;

protected:

    // keeping a reference to the parentPlot because this object is not responsible
    // of cleaning up dynamically allocated objects
    QCustomPlot& parentPlot_;
    QCPAxisRect axisRect_;

    bool autoScaleKeyAxis_;

    void removeIndicatorMenu(QPoint pos, QList<QCPAbstractPlottable*> plottables);

    // to create a new indicator entry, we first instantiate an iplot and get the
    // associated plottables. we create an entry and map the plottable to the new iplot.
    // when we select a graph in qcp for removal, we first get back a qcpabstractplottable
    // pointer from qcustomplot we can use this pointer to map to the iplot that it
    // corresponds to and remove the entry. refer to showMenu
    // we use a shared_ptr for multiple instances to the same iplot
    std::unordered_map<QCPAbstractPlottable*, std::shared_ptr<IIndicatorGraph>> activeIndicatorPlots_;

protected slots:

    void plotSelectSlot(bool selected);
};

#endif // IPLOT_H
