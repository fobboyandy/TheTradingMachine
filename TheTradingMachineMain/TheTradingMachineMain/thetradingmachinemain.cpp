#include "thetradingmachinemain.h"
#include "ui_thetradingmachinemain.h"

TheTradingMachineMain::TheTradingMachineMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TheTradingMachineMain)
{
    ui->setupUi(this);
}

TheTradingMachineMain::~TheTradingMachineMain()
{
    delete ui;
}
