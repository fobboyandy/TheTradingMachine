#include "thetradingmachinetabs.h"


TheTradingMachineTabs::TheTradingMachineTabs(QWidget* parent) :
    QWidget(parent),
    gridLayout(nullptr),
    horizontalScrollBar(nullptr),
    verticalScrollBar(nullptr),
    plot(nullptr)

{
    this->setObjectName(QStringLiteral("tab"));
    gridLayout = new QGridLayout(this);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));
    horizontalScrollBar = new QScrollBar(this);
    horizontalScrollBar->setObjectName(QStringLiteral("horizontalScrollBar"));
    horizontalScrollBar->setOrientation(Qt::Horizontal);

    gridLayout->addWidget(horizontalScrollBar, 1, 0, 1, 1);

    plot = new QCustomPlot(this);
    plot->setObjectName(QStringLiteral("plot"));

    gridLayout->addWidget(plot, 0, 0, 1, 1);

    verticalScrollBar = new QScrollBar(this);
    verticalScrollBar->setObjectName(QStringLiteral("verticalScrollBar"));
    verticalScrollBar->setOrientation(Qt::Vertical);

    gridLayout->addWidget(verticalScrollBar, 0, 1, 1, 1);
}

TheTradingMachineTabs::~TheTradingMachineTabs()
{
    //parent will free
}


