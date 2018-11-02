#include "thetradingmachinemainwindow.h"
#include "ui_thetradingmachinemainwindow.h"

#include <Windows.h>
#include <QString>
#include <QInputDialog>
#include <type_traits>
#include "../InteractiveBrokersClient/InteractiveBrokersClient/InteractiveBrokersClient.h"

// instantiation of static members
std::shared_ptr<InteractiveBrokersClient> TheTradingMachineMainWindow::client_ = nullptr;
QTimer TheTradingMachineMainWindow::clientReadyTimer_;
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

    // once connected, all additional sessions will be connected
    if(client_ != nullptr)
    {
        client_->isReady() ?
                    ui->actionConnect->setChecked(true) :
                    ui->actionConnect->setEnabled(false);
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
    if(newTab->valid())
    {
        ui->tabWidget->addTab(newTab, newTab->tabName());
    }
    else
    {
        delete newTab;
    }
}

void TheTradingMachineMainWindow::stopCurrentSession()
{

}

void TheTradingMachineMainWindow::connectInteractiveBroker()
{
    if(client_ == nullptr)
    {
        // we don't need to check if client is null because
        // this action is disabled in the destructor when
        // client is already connected. so client must
        // be null if this code is reachable
        client_ = GetInteractiveBrokersClient();
        if(client_ != nullptr)
        {
            qDebug("retrieving client connection ");
            // disable the button. it will be reenabled by checkInteractiveBrokerConnection once
            // the connection is established
            connect(&clientReadyTimer_, &QTimer::timeout, this, &TheTradingMachineMainWindow::checkInteractiveBrokerConnection);
            ui->actionConnect->setEnabled(false);
            clientReadyTimer_.start(100);
        }
    }

    //if it's connected, keep it connected
    else if(ui->actionConnect->isChecked())
    {
        ui->actionConnect->setChecked(true);
    }
    qDebug("triggered");
}

void TheTradingMachineMainWindow::closeAll()
{

}

void TheTradingMachineMainWindow::closeTab(int tabIndex)
{
    auto tabPtr = ui->tabWidget->widget(tabIndex);
    tabPtr->deleteLater();
}

void TheTradingMachineMainWindow::checkInteractiveBrokerConnection()
{
    if(client_->isReady())
    {
        if(!ui->actionConnect->isEnabled())
        {
            ui->actionConnect->setEnabled(true);
        }
        if(!ui->actionConnect->isChecked())
        {
            ui->actionConnect->setChecked(true);
        }
    }
}

void TheTradingMachineMainWindow::connectDefaulSlots()
{
    connect(ui->actionNew_Session, &QAction::triggered, this, &TheTradingMachineMainWindow::newSession);
    connect(ui->actionConnect, &QAction::triggered, this, &TheTradingMachineMainWindow::connectInteractiveBroker);
    connect(ui->actionPlay, &QAction::triggered, this, &TheTradingMachineMainWindow::play);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &TheTradingMachineMainWindow::closeTab);
    connect(&clientReadyTimer_, &QTimer::timeout, this, &TheTradingMachineMainWindow::checkInteractiveBrokerConnection);
}

void TheTradingMachineMainWindow::simpleMovingAveragePrice()
{
}

void TheTradingMachineMainWindow::simpleMovingAverageVolume()
{

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
            TheTradingMachineTab::AlgorithmApi::StopAlgorithmFnPtr stopAlgorithmProcAddr = reinterpret_cast<TheTradingMachineTab::AlgorithmApi::StopAlgorithmFnPtr>(GetProcAddress(dllHndl_, "StopAlgorithm"));

            //check if any functions are invalid
            if(playAlgorithmProcAddr == nullptr ||
               stopAlgorithmProcAddr == nullptr)
            {
                displayMessageBox("Failed to load all the necessary functions from the provided algorithm.");
            }
            else
            {
                api_.playAlgorithm = [=](std::string ticker, std::shared_ptr<PlotData>* plotData, std::shared_ptr<InteractiveBrokersClient> ibIntf, bool live)
                {
                    return playAlgorithmProcAddr(ticker, plotData, ibIntf, live);
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
