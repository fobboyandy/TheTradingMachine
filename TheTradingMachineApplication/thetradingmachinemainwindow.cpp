#include "thetradingmachinemainwindow.h"
#include "ui_thetradingmachinemainwindow.h"

#include <Windows.h>
#include <QString>

#include <type_traits>

// instantiation of static members
IBInterfaceClient* TheTradingMachineMainWindow::ibInterface_ = nullptr;
std::unordered_set<std::wstring> TheTradingMachineMainWindow::algorithmInstances_;

TheTradingMachineMainWindow::TheTradingMachineMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TheTradingMachineMainWindow),
    dllHndl_(nullptr),
    valid_(true)
{
    ui->setupUi(this);
    this->show();

    // We set this attribute because we don't keep a handle to each new window.
    // Therefore, we need a higher level application to free the memory. This function
    // fully releases the memory (equivalent to delete this) which of course invokes
    // the destructor and properly destruct the members.
    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->actionNew_Session, &QAction::triggered, this, &TheTradingMachineMainWindow::newSession);

    dllFile_ = QFileDialog::getOpenFileName(this, "Load Algorithm", QString(), "*.dll").toStdWString();

    auto displayMessageBox = [&](const QString&& msg)
    {
        QMessageBox prompt;
        prompt.setText(msg);
        prompt.exec();
    };

    if(dllFile_.length() > 0)
    {
        if(algorithmInstances_.find(dllFile_) != algorithmInstances_.end())
        {
            displayMessageBox("A session with this algorithm is already open!");
            valid_ = false;
            return;
        }

        algorithmInstances_.insert(dllFile_);

        dllHndl_ = LoadLibrary(dllFile_.c_str());

        if(dllHndl_ != nullptr)
        {
            using PlayAlgorithmFnPtr = int (*)(std::string, IBInterfaceClient*);
            using GetPlotDataFn = bool (*)(int, SupportBreakShortPlotData::PlotData**);
            using StopAlgorithmFnPtr = bool (*)(int);

            PlayAlgorithmFnPtr playAlgorithmProcAddr = reinterpret_cast<PlayAlgorithmFnPtr>(GetProcAddress(dllHndl_, "PlayAlgorithm"));
            GetPlotDataFn getPlotDataProcAddr = reinterpret_cast<GetPlotDataFn>(GetProcAddress(dllHndl_, "GetPlotData"));
            StopAlgorithmFnPtr stopAlgorithmProcAddr = reinterpret_cast<StopAlgorithmFnPtr>(GetProcAddress(dllHndl_, "StopAlgorithm"));

            if(playAlgorithmProcAddr == nullptr ||
               getPlotDataProcAddr == nullptr ||
               stopAlgorithmProcAddr == nullptr)
            {
                displayMessageBox("Failed to load all the necessary functions from the provided algorithm.");
                valid_ = false;
            }

            else
            {
                playAlgorithm = [=](std::string ticker, IBInterfaceClient* ibIntf)
                {
                    return playAlgorithmProcAddr(ticker, ibIntf);
                };

                getPlotData = [=](int inst, SupportBreakShortPlotData::PlotData** plotDataOut)
                {
                    return getPlotDataProcAddr(inst, plotDataOut);
                };

                stopAlgorithm = [=](int inst)
                {
                    return stopAlgorithmProcAddr(inst);
                };
                displayMessageBox("Succcessfully loaded the algorithm!");
            }
        }
        else
        {
            displayMessageBox("Failed to load algorithm.");
            valid_ = false;
        }
    }
    else
    {
        displayMessageBox("An algorithm was not provided.");
        valid_ = false;
    }
}

TheTradingMachineMainWindow::~TheTradingMachineMainWindow()
{
    qDebug("destructing");
    delete ui;
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
        newWindowSession->close();
    }
}

void TheTradingMachineMainWindow::play()
{

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


