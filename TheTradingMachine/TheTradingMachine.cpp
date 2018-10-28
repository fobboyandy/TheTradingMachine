#include <iostream>
#include <functional>
#include <atomic>
#include <sstream>
#include <fstream>
#include <thread>
#include <assert.h>
#include "TheTradingMachine.h"
#include "LocalBroker.h"

#define TICK_CSV_ROW_SZ 12

class TheTradingMachine::TheTradingMachineImpl
{
public:
	explicit TheTradingMachineImpl(TheTradingMachine* parent, std::string input, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live);
	~TheTradingMachineImpl();
	std::shared_ptr<PlotData> plotData;
	void run();
	void stop();

private:
	TheTradingMachine* parent;
	LocalBroker localBroker;

	CallbackHandle callbackHandle;
	void tickHandler(const Tick& tick);
};

TheTradingMachine::TheTradingMachineImpl::TheTradingMachineImpl(TheTradingMachine* parentIn, std::string input, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live) :
	parent(parentIn),
	plotData(std::make_shared<PlotData>()),
	localBroker(input, ibApiPtr, live)
{
	callbackHandle = localBroker.registerCallback([this](const Tick& tick)
	{
		this->tickHandler(tick);
	});
}

TheTradingMachine::TheTradingMachineImpl::~TheTradingMachineImpl()
{
	stop();
}

void TheTradingMachine::TheTradingMachineImpl::run()
{
	localBroker.run();
}

void TheTradingMachine::TheTradingMachineImpl::stop()
{
	//unregistering the callback stops the algorithm
	localBroker.unregisterCallback(callbackHandle);
}


void TheTradingMachine::TheTradingMachineImpl::tickHandler(const Tick & tick)
{
	// first handle the tick algorithm specific callback
	parent->tickHandler(tick);

	// in case if gui is sometimes slow and is holding the lock, we don't want to block this thread
	// therefore, if we fail to retrieve the lock immediately, we push the data into a secondary buffer.
	// when we successfully get the lock, we simply have to move the data into plotData. this will allow
	// the algorithm to be more overall responsive .
	std::unique_lock<std::mutex> lock(plotData->plotDataMtx);
	PlotData& plotDataRef = *plotData;
	//this is the only thread that accesses buffer no synchronization needed
	plotData->ticks.push_back(tick);
}

TheTradingMachine::TheTradingMachine(std::string input, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live):
	_impl(new TheTradingMachineImpl(this, input, ibApiPtr, live))
{
}

TheTradingMachine::~TheTradingMachine()
{
	if (_impl != nullptr)
	{
		delete _impl;
		_impl = nullptr;
	}
}

std::shared_ptr<PlotData> TheTradingMachine::getPlotData()
{
	return _impl->plotData;
}

void TheTradingMachine::run()
{
	if (_impl != nullptr)
	{
		_impl->run();
	}
}

void TheTradingMachine::stop()
{
	if (_impl != nullptr)
	{
		_impl->stop();
	}
}
