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
#include "../InteractiveBrokersClient/InteractiveBrokersClient/InteractiveBrokersClient.h"
#include "thetradingmachinetab.h"


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

// toolbar controls
private slots:
    void newSession();
    void play();
    void stopCurrentSession();
    void connectInteractiveBroker();
    void closeAll();
    void closeTab(int tabIndex);
    void checkInteractiveBrokerConnection();

//members
private:
    Ui::TheTradingMachineMainWindow *ui;

    // IB Connection. we only need one for all sessions
    static std::shared_ptr<InteractiveBrokersClient> client_;
    static QTimer clientReadyTimer_;

    // monitor for duplicate instances of the same algorithm
    static std::unordered_set<std::wstring> algorithmInstances_;

    // dll interface
    std::wstring dllFile_;
    HMODULE dllHndl_;
    TheTradingMachineTab::AlgorithmApi api_;

    // currentTab
    TheTradingMachineTab* currentTab;

    bool valid_;

// functions
    bool promptLoadAlgorithm();
    void connectDefaulSlots();


// indicator slots
private slots:
    void simpleMovingAveragePrice(void);
    void simpleMovingAverageVolume(void);

};

#endif // THETRADINGMACHINEMAINWINDOW_H
