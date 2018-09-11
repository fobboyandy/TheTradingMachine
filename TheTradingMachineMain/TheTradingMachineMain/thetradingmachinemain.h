#ifndef THETRADINGMACHINEMAIN_H
#define THETRADINGMACHINEMAIN_H

#include <QMainWindow>

namespace Ui {
class TheTradingMachineMain;
}

class TheTradingMachineMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit TheTradingMachineMain(QWidget *parent = nullptr);
    ~TheTradingMachineMain();

private:
    Ui::TheTradingMachineMain *ui;
};

#endif // THETRADINGMACHINEMAIN_H
