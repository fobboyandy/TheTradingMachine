#include "thetradingmachinemainwindow.h"
#include "ui_thetradingmachinemainwindow.h"

#include <Windows.h>
#include <QString>
#include <QInputDialog>

#include <type_traits>

// instantiation of static members
std::shared_ptr<IBInterfaceClient> TheTradingMachineMainWindow::client_ = nullptr;
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
    // if we are destructing main window, we need to unload the library
    // right here. Therefore, we cannot defer the destruction of the tabs
    // because TheTradingMachineTabs will try to access deleted memory
    // (result of FreeLibrary)

    // deleting children explicitly is not recommended but necessary here
    // deleting the tabWidget will remove all the tabs
    delete ui->tabWidget;

    qDebug("window destruct");
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
    auto objects = ui->tabWidget->children();
    TheTradingMachineTab* newTab = new TheTradingMachineTab(api_, client_, nullptr);
    ui->tabWidget->addTab(newTab, std::string("test").c_str());

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
    auto tabPtr = ui->tabWidget->widget(tabIndex);
    tabPtr->deleteLater();
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
            TheTradingMachineTab::AlgorithmApi::PlayAlgorithmFnPtr playAlgorithmProcAddr = reinterpret_cast<TheTradingMachineTab::AlgorithmApi::PlayAlgorithmFnPtr>(GetProcAddress(dllHndl_, "PlayAlgorithm"));
            TheTradingMachineTab::AlgorithmApi::GetPlotDataFnPtr getPlotDataProcAddr = reinterpret_cast<TheTradingMachineTab::AlgorithmApi::GetPlotDataFnPtr>(GetProcAddress(dllHndl_, "GetPlotData"));
            TheTradingMachineTab::AlgorithmApi::StopAlgorithmFnPtr stopAlgorithmProcAddr = reinterpret_cast<TheTradingMachineTab::AlgorithmApi::StopAlgorithmFnPtr>(GetProcAddress(dllHndl_, "StopAlgorithm"));

            //check if any functions are invalid
            if(playAlgorithmProcAddr == nullptr ||
               getPlotDataProcAddr == nullptr ||
               stopAlgorithmProcAddr == nullptr)
            {
                displayMessageBox("Failed to load all the necessary functions from the provided algorithm.");
            }
            else
            {
                api_.playAlgorithm = [=](std::string ticker, std::shared_ptr<IBInterfaceClient> ibIntf)
                {
                    return playAlgorithmProcAddr(ticker, ibIntf);
                };

                api_.getPlotData = [=](int inst, std::shared_ptr<PlotData>* plotDataOut)
                {
                    return getPlotDataProcAddr(inst, plotDataOut);
                };

                api_.stopAlgorithm = [=](int inst)
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
