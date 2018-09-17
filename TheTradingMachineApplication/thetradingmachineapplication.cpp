#include "thetradingmachineapplication.h"
#include "ui_thetradingmachineapplication.h"

TheTradingMachineApplication::TheTradingMachineApplication(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TheTradingMachineApplication)
{
    ui->setupUi(this);
    ui->plot->addGraph();
    ui->plot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    ui->plot->rescaleAxes();
    replotTimer.start(50);
}

TheTradingMachineApplication::~TheTradingMachineApplication()
{
    delete ui;
}