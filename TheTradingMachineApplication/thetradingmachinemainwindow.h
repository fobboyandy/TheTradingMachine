#ifndef THETRADINGMACHINEMAINWINDOW_H
#define THETRADINGMACHINEMAINWINDOW_H

// Qt
#include <QMainWindow>
#include <QTimer>

// STL
#include <string>
#include <functional>
#include <unordered_set>
#include <memory>

// Windows
#include <Windows.h>

// The Trading Machine
#include "TheTradingMachine.h"
#include "thetradingmachinetabs.h"


namespace Ui {
class TheTradingMachineMainWindow;
}

class TheTradingMachineMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TheTradingMachineMainWindow(QWidget *parent = nullptr);
    ~TheTradingMachineMainWindow();
    bool valid();

private slots:
    void newSession();
    void play();
    void stopCurrentSession();
    void connectInteractiveBroker();
    void closeAll();
    void closeTab(int tabIndex);

//members
private:
    Ui::TheTradingMachineMainWindow *ui;

    // IB Connection. Only one allowed for all sessions
    static IBInterfaceClient* ibInterface_;
    static std::unordered_set<std::wstring> algorithmInstances_;

    // dll interface
    std::wstring dllFile_;
    HMODULE dllHndl_;
    std::function<int(std::string, IBInterfaceClient*)> playAlgorithm;
    std::function<bool(int, std::shared_ptr<PlotData>**)> getPlotData;
    std::function<bool(int)> stopAlgorithm;

    bool valid_;

//functions
    bool promptLoadAlgorithm();
    void connectDefaulSlots();
};

#endif // THETRADINGMACHINEMAINWINDOW_H
