#include "thetradingmachinemainwindow.h"
#include "ui_thetradingmachinemainwindow.h"

#include <Windows.h>
#include <QString>
#include <QInputDialog>

#include <type_traits>

// instantiation of static members
IBInterfaceClient* TheTradingMachineMainWindow::ibInterface_ = nullptr;
std::unordered_set<std::wstring> TheTradingMachineMainWindow::algorithmInstances_;

TheTradingMachineMainWindow::TheTradingMachineMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TheTradingMachineMainWindow),
    dllHndl_(nullptr),
    valid_(false)
{
    ui->setupUi(this);
    // the tab was created in qt designer as a template. we will use the generated code
    // to programmatically add/remove tabs
    ui->tabWidget->removeTab(0);
    this->show();

    // We set this attribute because we don't keep a handle to each new window.
    // Therefore, we need a higher level application to free the memory. This function
    // fully releases the memory (equivalent to delete this) which of course invokes
    // the destructor and properly destruct the members.
    this->setAttribute(Qt::WA_DeleteOnClose);

    if(promptLoadAlgorithm())
    {
        // valid_ is used in promptLoadAlgorithm as if it was a local variable. setting here
        // to increase readability
        valid_ = true;
        connectDefaulSlots();
    }
}

TheTradingMachineMainWindow::~TheTradingMachineMainWindow()
{
    delete ui;
    if(dllHndl_ != nullptr)
        FreeLibrary(dllHndl_);
}

bool TheTradingMachineMainWindow::valid()
{
    return valid_;
}

void TheTradingMachineMainWindow::newSession()
{
    auto newWindowSession = new TheTradingMachineMainWindow;
    if(!newWindowSession->valid())
    {
        //dont need to delete memory. handled by Qapplication
        newWindowSession->close();
    }
}

void TheTradingMachineMainWindow::play()
{
    //prompt user for the input method. real time or historical ticks
    std::string fpTest("D:\\Users\\fobboyandy\\Desktop\\TheTradingMachine\\outputfiles\\Jul 17AMD.tickdat");
    //if real time, check for ib connection

    // instantiate the algorithm for this ticker    
    int algorithmHandle = playAlgorithm(fpTest, ibInterface_);

    // retrieve the plot data and assign it to the tab
    PlotData* plotDataOut = nullptr;
    if(getPlotData(algorithmHandle, &plotDataOut) && plotDataOut != nullptr)
    {
        // set up a tab for the current algorithm
        std::string tabname("Jul 17AMD");
        TheTradingMachineTabs* newTab = new TheTradingMachineTabs(nullptr);
        newTab->setPlotData(algorithmHandle, plotDataOut);
        newTab->plot()->addGraph();

        ui->tabWidget->addTab(newTab, tabname.c_str());

        // start the plot
        newTab->run();
    }
}

void TheTradingMachineMainWindow::stopCurrentSession()
{

}

void TheTradingMachineMainWindow::connectInteractiveBroker()
{

}

void TheTradingMachineMainWindow::closeAll()
{

}

void TheTradingMachineMainWindow::closeTab(int tabIndex)
{
    // todo: need to unregister the algorithm interactive broker as appropriate
    // maybe this should be handled in the algorithm side
    qDebug(std::to_string(tabIndex).c_str());
    auto tabPtr = ui->tabWidget->widget(tabIndex);
    if(tabPtr != nullptr)
    {
        int algorithmHandle = reinterpret_cast<TheTradingMachineTabs*>(tabPtr)->getHandle();
        if(stopAlgorithm(algorithmHandle))
        {
            tabPtr->deleteLater(); //safer way of deleting an object since there may be pending events in the event queue
            qDebug("Successfully ended algorithm");
        }
        else
        {
            qDebug("Unable to stop algorithm!!!");
        }
    }

}

void TheTradingMachineMainWindow::connectDefaulSlots()
{
    connect(ui->actionNew_Session, &QAction::triggered, this, &TheTradingMachineMainWindow::newSession);
    connect(ui->actionPlay, &QAction::triggered, this, &TheTradingMachineMainWindow::play);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &TheTradingMachineMainWindow::closeTab);
}

bool TheTradingMachineMainWindow::promptLoadAlgorithm()
{
    valid_ = false;
    // capture by reference since this lambda only used in this scope
    auto displayMessageBox = [&](const QString&& msg)
    {
        QMessageBox prompt;
        prompt.setText(msg);
        prompt.exec();
    };

    dllFile_ = QFileDialog::getOpenFileName(this, "Load Algorithm", QString(), "*.dll").toStdWString();
    if(algorithmInstances_.find(dllFile_) != algorithmInstances_.end())
    {
        displayMessageBox("A session with this algorithm is already open!");
    }
    else if(dllFile_.length() > 0)
    {
        dllHndl_ = LoadLibrary(dllFile_.c_str());
        if(dllHndl_ != nullptr)
        {
            using PlayAlgorithmFnPtr = int (*)(std::string, IBInterfaceClient*);
            using GetPlotDataFn = bool (*)(int, PlotData**);
            using StopAlgorithmFnPtr = bool (*)(int);

            PlayAlgorithmFnPtr playAlgorithmProcAddr = reinterpret_cast<PlayAlgorithmFnPtr>(GetProcAddress(dllHndl_, "PlayAlgorithm"));
            GetPlotDataFn getPlotDataProcAddr = reinterpret_cast<GetPlotDataFn>(GetProcAddress(dllHndl_, "GetPlotData"));
            StopAlgorithmFnPtr stopAlgorithmProcAddr = reinterpret_cast<StopAlgorithmFnPtr>(GetProcAddress(dllHndl_, "StopAlgorithm"));

            //check if any functions are invalid
            if(playAlgorithmProcAddr == nullptr ||
               getPlotDataProcAddr == nullptr ||
               stopAlgorithmProcAddr == nullptr)
            {
                displayMessageBox("Failed to load all the necessary functions from the provided algorithm.");
            }
            else
            {
                playAlgorithm = [=](std::string ticker, IBInterfaceClient* ibIntf)
                {
                    return playAlgorithmProcAddr(ticker, ibIntf);
                };

                getPlotData = [=](int inst, PlotData** plotDataOut)
                {
                    return getPlotDataProcAddr(inst, plotDataOut);
                };

                stopAlgorithm = [=](int inst)
                {
                    return stopAlgorithmProcAddr(inst);
                };
                displayMessageBox("Succcessfully loaded the algorithm!");
                // only mark the algorithm in the set if we succesfully loaded.
                algorithmInstances_.insert(dllFile_);
                valid_ = true;
            }
        }
        else
        {
            displayMessageBox("Failed to load algorithm.");
        }
    }
    else
    {
        displayMessageBox("An algorithm was not provided.");
    }

    return valid_;
}
