#pragma once

#include <thread>
#include <memory>
#include <functional>
#include <atomic>
#include <fstream>
#include <string>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include "../IBInterface/Tick.h"
#include "../IBInterface/bar.h"
#include "../IBInterfaceClient/IBInterfaceClient.h"

#ifdef EXPORTTHETRADINGMACHINEDLL
#define THETRADINGMACHINEDLL __declspec(dllexport)
#else
#define THETRADINGMACHINEDLL __declspec(dllimport)
#endif

// plot data structure shared with the gui. plot data is stored in this format.
// the gui is provided an address to the plot data struct and notified upon 
// new data
struct PlotData
{
	std::mutex plotDataMtx;
	bool finished;
	std::queue<Tick> buffer;
	std::vector<Tick> ticks;
	std::vector<std::string> action;
};

class THETRADINGMACHINEDLL TheTradingMachine
{
public:
	explicit TheTradingMachine(std::string in, std::shared_ptr<IBInterfaceClient> ibApiPtr = std::shared_ptr<IBInterfaceClient>(nullptr));
	virtual ~TheTradingMachine();
	std::shared_ptr<PlotData> getPlotPlotData();

private:
	class TheTradingMachineImpl;
	TheTradingMachineImpl* impl_;
protected:
	// we need the start and stop functions because tickHandler is pure virtual.
	// because TheTradingMachine starts a thread that runs on the derived classes'
	// tickHandler, we need the derived class to be able to stop our thread first
	// before their tickHandler goes out of scope (derived classes get destructed
	// before base classes) to prevent the running thread to access the function
	// of the destructed derived class
	void start(void);
	void stop(void);
	virtual void tickHandler(const Tick& tick) = 0;


	//let the impl class call the pure virtual tickHandler
	friend TheTradingMachineImpl;
};
