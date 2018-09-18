#include "thetradingmachineapplication.h"
#include <iostream>
#include <string>
#include <QApplication>
#include <Windows.h>
#include "SupportBreakShort/SupportBreakShortPlotData.h"

class IBInterfaceClient;
typedef int (*InitAlgorithmFuncPtr)(std::string dataInput, IBInterfaceClient * ibInst);
typedef bool (*GetPlotDataFuncPtr)(int instHandle, SupportBreakShortPlotData::PlotData** dataOut);
typedef bool (*CloseAlgorithmFuncPtr)(size_t instHandle);

static HMODULE dllHndl = nullptr;
static InitAlgorithmFuncPtr InitAlgorithm = nullptr;
static GetPlotDataFuncPtr GetPlotData = nullptr;
static CloseAlgorithmFuncPtr CloseAlgorithm = nullptr;

bool LoadAlgorithm(LPCWSTR dllName)
{
    if(dllHndl != nullptr)
        FreeLibrary(dllHndl);
    dllHndl = LoadLibrary(dllName);
    if(dllHndl != nullptr)
    {
        InitAlgorithm = reinterpret_cast<InitAlgorithmFuncPtr>(GetProcAddress(dllHndl, "InitAlgorithm"));
        GetPlotData = reinterpret_cast<GetPlotDataFuncPtr>(GetProcAddress(dllHndl, "GetPlotData"));
        CloseAlgorithm = reinterpret_cast<CloseAlgorithmFuncPtr>(GetProcAddress(dllHndl, "CloseAlgorithm"));

        if(InitAlgorithm == nullptr || GetPlotData == nullptr || CloseAlgorithm == nullptr)
        {
            std::cout << "Failed to load all the functions." << std::endl;
            FreeLibrary(dllHndl);
        }
        else
        {
            std::cout << "Successfully loaded all the functions." << std::endl;
            return true;
        }
    }
    else
    {
        std::cout << "Unable to load provided DLL." << std::endl;
    }

    return false;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(LoadAlgorithm(L"SupportBreakShort.dll"))
    {
        TheTradingMachineApplication w;
        w.show();

        int amdSbsHandle = InitAlgorithm("C:\\Users\\Andy\\Desktop\\TheTradingMachine\\outputfiles\\Jul 24AMD.tickdat", nullptr);
        SupportBreakShortPlotData::PlotData* amdSbsPlotData = nullptr;
        if(GetPlotData(amdSbsHandle, &amdSbsPlotData) && amdSbsPlotData != nullptr)
        {
            std::cout << "Got plot data" << std::endl;
            w.initializePlotData(amdSbsPlotData);
        }

        return a.exec();
    }

    return 1;
}
