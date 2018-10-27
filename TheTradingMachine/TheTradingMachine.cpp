#include <iostream>
#include <functional>
#include <atomic>
#include <sstream>
#include <fstream>
#include <thread>
#include <stdexcept>
#include "TheTradingMachine.h"
#include "LocalBroker.h"
#include "Common.h"

class TheTradingMachine::TheTradingMachineImpl
{
public:
	//paper trading using tickdata
	TheTradingMachineImpl(std::string tickDataFile);

	//live trading or paper trade using real time ticks.
	TheTradingMachineImpl(std::string ticker, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live = false);
	~TheTradingMachineImpl();
	void run();
	void setCallback(TickCallbackFunction callback);

	// public plotData so that algorithm can post algorithm specific plot data
	std::shared_ptr<PlotData> plotData;

//ordering api
public:
	PositionId buyMarketNoStop(std::string ticker, int numShares);
	PositionId buyMarketStopMarket(std::string ticker, int numShares, double stopPrice);
	PositionId buyMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId buyLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId buyLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	PositionId sellMarketNoStop(std::string ticker, int numShares);
	PositionId sellMarketStopMarket(std::string ticker, int numShares, double activationPrice);
	PositionId sellMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId sellLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId sellLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

private:
	TickCallbackFunction algorithmTickCallback;
	std::shared_ptr<IBInterfaceClient> ibApi;
	void engineTickHandler(const Tick& tick);
	LocalBroker localBroker;
};

TheTradingMachine::TheTradingMachineImpl::TheTradingMachineImpl(std::string tickDataFile):
	plotData(std::make_shared<PlotData>()),
	localBroker(tickDataFile)
{
	if (localBroker.valid())
	{
		auto callback = [this](const Tick& tick)
		{
			this->engineTickHandler(tick);
		};
		//we don't care about the returned handle because we won't be unregistering this callback
		localBroker.registerCallback(callback);
	}
	else
	{
		throw std::runtime_error("Failed to initialize broker.");
	}
}

TheTradingMachine::TheTradingMachineImpl::TheTradingMachineImpl(std::string ticker, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live) :
	ibApi(ibApiPtr),
	plotData(std::make_shared<PlotData>()), //plot data must be initialized before preTickHandler is called
	localBroker(ticker, ibApiPtr, live)
{
	if(localBroker.valid())
	{
		auto callback = [this](const Tick& tick)
		{
			this->engineTickHandler(tick);
		};
		//we don't care about the returned handle because we won't be unregistering this callback
		localBroker.registerCallback(callback);
	}
	else
	{
		throw std::runtime_error("Failed to initialize broker.");
	}
}

TheTradingMachine::TheTradingMachineImpl::~TheTradingMachineImpl()
{
}

void TheTradingMachine::TheTradingMachineImpl::run()
{
	localBroker.run();
}

void TheTradingMachine::TheTradingMachineImpl::setCallback(TickCallbackFunction callback)
{
	algorithmTickCallback = callback;
}

void TheTradingMachine::TheTradingMachineImpl::engineTickHandler(const Tick & tick)
{
	//let algorithm act on the data first
	algorithmTickCallback(tick);

	// in case if gui is sometimes slow and is holding the lock, we don't want to block this thread
	// therefore, if we fail to retrieve the lock immediately, we push the data into a secondary buffer.
	// when we successfully get the lock, we simply have to move the data into plotData. this will allow
	// the algorithm to be more overall responsive .
	std::unique_lock<std::mutex> lock(plotData->plotDataMtx);
	PlotData& plotDataRef = *plotData;
	plotDataRef.ticks.push_back(tick);
}

PositionId TheTradingMachine::TheTradingMachineImpl::buyMarketNoStop(std::string ticker, int numShares)
{
	return localBroker.buyMarketNoStop(ticker, numShares);
}

PositionId TheTradingMachine::TheTradingMachineImpl::buyMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::buyMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::buyLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::buyLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::sellMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::sellMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::sellMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::sellLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::sellLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

TheTradingMachine::TheTradingMachine(std::string tickDataFile):
	_impl(new TheTradingMachineImpl(tickDataFile))
{
}

TheTradingMachine::TheTradingMachine(std::string ticker, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live):
	_impl(new TheTradingMachineImpl(ticker, ibApiPtr, live))
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

void TheTradingMachine::setCallback(TickCallbackFunction callback)
{
	_impl->setCallback(callback);
}

void TheTradingMachine::run()
{
	_impl->run();
}

PositionId TheTradingMachine::buyMarketNoStop(std::string ticker, int numShares)
{
	return _impl->buyMarketNoStop(ticker, numShares);
}

PositionId TheTradingMachine::buyMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::buyMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::buyLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::buyLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::sellMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
}

PositionId TheTradingMachine::sellMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::sellMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::sellLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::sellLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}
