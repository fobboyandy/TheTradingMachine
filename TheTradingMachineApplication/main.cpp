#include <QApplication>
#include "thetradingmachinemainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto mainWindow = new TheTradingMachineMainWindow;

    // ensure that the first window is good
    if(mainWindow->valid())
        return a.exec();

    // we don't need to worry about freeing the memory because each window self destructs

    return -1;
}
