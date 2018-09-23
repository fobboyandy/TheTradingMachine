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
    QTimer tabReplotTimer_;

    // We need this variable to keep track of number of tabs
    // so that we can stop the timer. Because we use deleteLater
    // to close a tab, using the count() function from the tabs
    // widget will be insufficient since we don't know when
    // the delete occurs. Count() may return a value > 0 even though
    // tabs are already scheduled for deletion. This would cause a false
    // condition to be true and the timer never stops.
    int tabsCount_;

//functions
    bool promptLoadAlgorithm();
    void connectDefaulSlots();
};

#endif // THETRADINGMACHINEMAINWINDOW_H
