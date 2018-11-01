#include <QApplication>
#include <Windows.h>
#include "thetradingmachinemainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    auto mainWindow = new TheTradingMachineMainWindow;

    // ensure that the first window is good
    if(mainWindow->valid())
    {
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
        auto executionComplete = a.exec();
        SetThreadExecutionState(ES_CONTINUOUS);
        return executionComplete;
    }
    // we don't need to worry about freeing the memory because each window self destructs

    return -1;
}
