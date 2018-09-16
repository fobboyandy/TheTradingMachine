#include "thetradingmachineapplication.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TheTradingMachineApplication w;
    w.show();

    return a.exec();
}
