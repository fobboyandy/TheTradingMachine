#ifndef THETRADINGMACHINEMAINWINDOW_H
#define THETRADINGMACHINEMAINWINDOW_H

// Qt
#include <QMainWindow>

// STL
#include <string>
#include <functional>
#include <unordered_set>

// Windows
#include <Windows.h>

// The Trading Machine
#include "TheTradingMachine.h"
#include "SupportBreakShort/SupportBreakShortPlotData.h"


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


    void operator delete(void * p)
     {
        static int i = 0;
         qDebug(std::string(std::string("delete") + std::to_string(++i)).c_str());
     }

private slots:
    void newSession();
    void play();
    void stopCurrentSession();
    void connectInteractiveBroker();
    void closeAll();

private:
    Ui::TheTradingMachineMainWindow *ui;

    // IB Connection. Only one allowed for all sessions
    static IBInterfaceClient* ibInterface_;
    static std::unordered_set<std::wstring> algorithmInstances_;

    std::wstring dllFile_;
    HMODULE dllHndl_;

    std::function<int(std::string, IBInterfaceClient*)> playAlgorithm;
    std::function<bool(int, SupportBreakShortPlotData::PlotData**)> getPlotData;
    std::function<bool(int)> stopAlgorithm;

    bool valid_;
};

#endif // THETRADINGMACHINEMAINWINDOW_H
