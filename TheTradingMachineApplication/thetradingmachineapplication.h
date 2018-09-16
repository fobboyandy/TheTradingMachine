#ifndef THETRADINGMACHINEAPPLICATION_H
#define THETRADINGMACHINEAPPLICATION_H

#include <QMainWindow>

namespace Ui {
class TheTradingMachineApplication;
}

class TheTradingMachineApplication : public QMainWindow
{
    Q_OBJECT

public:
    explicit TheTradingMachineApplication(QWidget *parent = nullptr);
    ~TheTradingMachineApplication();

private:
    Ui::TheTradingMachineApplication *ui;
};

#endif // THETRADINGMACHINEAPPLICATION_H
