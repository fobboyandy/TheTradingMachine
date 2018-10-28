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
	PositionId longMarketNoStop(std::string ticker, int numShares);
	PositionId longMarketStopMarket(std::string ticker, int numShares, double stopPrice);
	PositionId longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId longLimitStopMarket(std::string ticker, int numShares, double entryLimit, double activationPrice);
	PositionId longLimitStopLimit(std::string ticker, int numShares, double entryLimit, double activationPrice, double stopLimit);

	PositionId shortMarketNoStop(std::string ticker, int numShares);
	PositionId shortMarketStopMarket(std::string ticker, int numShares, double activationPrice);
	PositionId shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double stopLimit);
	PositionId shortLimitStopMarket(std::string ticker, int numShares, double entryLimit, double activationPrice);
	PositionId shortLimitStopLimit(std::string ticker, int numShares, double entryLimit, double activationPrice, double stopLimit);
	
	void closePosition(PositionId posId);
	void reducePosition(PositionId posId, int numShares);
	Position getPosition(PositionId posId);
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

PositionId TheTradingMachine::TheTradingMachineImpl::longMarketNoStop(std::string ticker, int numShares)
{
	return localBroker.longMarketNoStop(ticker, numShares);
}

PositionId TheTradingMachine::TheTradingMachineImpl::longMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double stopLimit)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::longLimitStopMarket(std::string ticker, int numShares, double entryLimit, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::longLimitStopLimit(std::string ticker, int numShares, double entry, double activationPrice, double stopLimit)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::shortMarketNoStop(std::string ticker, int numShares)
{
	return localBroker.shortMarketNoStop(ticker, numShares);
}

PositionId TheTradingMachine::TheTradingMachineImpl::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double stopLimit)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::shortLimitStopMarket(std::string ticker, int numShares, double entryLimit, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::shortLimitStopLimit(std::string ticker, int numShares, double entryLimit, double activationPrice, double stopLimit)
{
	return PositionId();
}

void TheTradingMachine::TheTradingMachineImpl::closePosition(PositionId posId)
{
	localBroker.closePosition(posId);
}

void TheTradingMachine::TheTradingMachineImpl::reducePosition(PositionId posId, int numShares)
{
	localBroker.reducePosition(posId, numShares);
}

Position TheTradingMachine::TheTradingMachineImpl::getPosition(PositionId posId)
{
	return localBroker.getPosition(posId);
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

PositionId TheTradingMachine::longMarketNoStop(std::string ticker, int numShares)
{
	return _impl->longMarketNoStop(ticker, numShares);
}

PositionId TheTradingMachine::longMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::shortMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
}

PositionId TheTradingMachine::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

void TheTradingMachine::closePosition(PositionId posId)
{
	_impl->closePosition(posId);
}

void TheTradingMachine::reducePosition(PositionId posId, int numShares)
{
	_impl->reducePosition(posId, numShares);
}

Position TheTradingMachine::getPosition(PositionId posId)
{
	return _impl->getPosition(posId);
}
