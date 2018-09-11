#include "thetradingmachinemain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TheTradingMachineMain w;
    w.show();

    return a.exec();
}
