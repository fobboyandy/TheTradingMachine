#include "thetradingmachineapplication.h"
#include "ui_thetradingmachineapplication.h"

TheTradingMachineApplication::TheTradingMachineApplication(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TheTradingMachineApplication)
{
    ui->setupUi(this);
}

TheTradingMachineApplication::~TheTradingMachineApplication()
{
    delete ui;
}
