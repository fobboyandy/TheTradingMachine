#ifndef THETRADINGMACHINETABS_H
#define THETRADINGMACHINETABS_H

#include <QWidget>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QGridLayout>
#include "qcustomplot.h"

// this is a tab set up for the tab pages in the trading machine
class TheTradingMachineTabs : public QWidget
{
public:
    TheTradingMachineTabs(QWidget* parent = nullptr);
    ~TheTradingMachineTabs();

    // deleted functions because there is no need to duplicate a tab that looks like another
    TheTradingMachineTabs(const TheTradingMachineTabs& other) = delete;
    TheTradingMachineTabs(const TheTradingMachineTabs&& other) = delete;
    TheTradingMachineTabs& operator=(const TheTradingMachineTabs& other) = delete;

private:
    QGridLayout *gridLayout;
    QScrollBar *horizontalScrollBar;
    QScrollBar *verticalScrollBar;
    QCustomPlot *plot;
};

#endif // THETRADINGMACHINETABS_H
