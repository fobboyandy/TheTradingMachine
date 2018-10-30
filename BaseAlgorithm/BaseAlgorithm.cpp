#include <iostream>
#include <functional>
#include <atomic>
#include <sstream>
#include <fstream>
#include <thread>
#include <assert.h>
#include "BaseAlgorithm.h"
#include "LocalBroker.h"

#define TICK_CSV_ROW_SZ 12

class BaseAlgorithm::BaseAlgorithmImpl
{
public:
	explicit BaseAlgorithmImpl(BaseAlgorithm* parent, std::string input, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live);
	~BaseAlgorithmImpl();
	std::shared_ptr<PlotData> plotData;
	void run();
	void stop();

// ordering functions
public:
	PositionId longMarketNoStop(std::string ticker, int numShares);
	PositionId longMarketStopMarket(std::string ticker, int numShares, double stopPrice);
	PositionId longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	PositionId shortMarketNoStop(std::string ticker, int numShares);
	PositionId shortMarketStopMarket(std::string ticker, int numShares, double activationPrice);
	PositionId shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	void closePosition(PositionId posId);
	void reducePosition(PositionId posId, int numShares);
	Position getPosition(PositionId posId);

private:
	BaseAlgorithm* parent;
	LocalBroker localBroker;
	bool running;

	CallbackHandle callbackHandle;
	void tickHandler(const Tick& tick);
};

BaseAlgorithm::BaseAlgorithmImpl::BaseAlgorithmImpl(BaseAlgorithm* parentIn, std::string input, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live) :
	parent(parentIn),
	plotData(std::make_shared<PlotData>()),
	localBroker(input, ibApiPtr, live),
	running(false)
{
	callbackHandle = localBroker.registerCallback([this](const Tick& tick)
	{
		this->tickHandler(tick);
	});
}

BaseAlgorithm::BaseAlgorithmImpl::~BaseAlgorithmImpl()
{
	stop();
}

void BaseAlgorithm::BaseAlgorithmImpl::run()
{
	if(!running)
	{
		running = true;
		localBroker.run();
	}
}

void BaseAlgorithm::BaseAlgorithmImpl::stop()
{
	if (running)
	{
		running = false;
		//unregistering the callback stops the algorithm
		localBroker.unregisterCallback(callbackHandle);
	}
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longMarketNoStop(std::string ticker, int numShares)
{
	return localBroker.longMarketNoStop(ticker, numShares);
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return localBroker.longMarketStopMarket(ticker, numShares, stopPrice);
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return localBroker.longMarketStopLimit(ticker, numShares, activationPrice, limitPrice);
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return localBroker.longLimitStopMarket(ticker, numShares, buyLimit, activationPrice);
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return localBroker.longLimitStopLimit(ticker, numShares, buyLimit, activationPrice, limitPrice);
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortMarketNoStop(std::string ticker, int numShares)
{
	return localBroker.shortMarketNoStop(ticker, numShares);
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return localBroker.shortMarketStopMarket(ticker, numShares, activationPrice);
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return localBroker.shortMarketStopLimit(ticker, numShares, activationPrice, limitPrice);
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return localBroker.shortLimitStopMarket(ticker, numShares, buyLimit, activationPrice);
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return localBroker.shortLimitStopLimit(ticker, numShares, buyLimit, activationPrice, limitPrice);
}

void BaseAlgorithm::BaseAlgorithmImpl::closePosition(PositionId posId)
{
	return localBroker.closePosition(posId);
}

void BaseAlgorithm::BaseAlgorithmImpl::reducePosition(PositionId posId, int numShares)
{
	return localBroker.reducePosition(posId, numShares);
}

Position BaseAlgorithm::BaseAlgorithmImpl::getPosition(PositionId posId)
{
	return localBroker.getPosition(posId);
}

void BaseAlgorithm::BaseAlgorithmImpl::tickHandler(const Tick & tick)
{
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

BaseAlgorithm::BaseAlgorithm(std::string input, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live):
	_impl(new BaseAlgorithmImpl(this, input, ibApiPtr, live))
{
}

BaseAlgorithm::~BaseAlgorithm()
{
	if (_impl != nullptr)
	{
		delete _impl;
		_impl = nullptr;
	}
}

std::shared_ptr<PlotData> BaseAlgorithm::getPlotData()
{
	return _impl->plotData;
}

void BaseAlgorithm::run()
{
	if (_impl != nullptr)
	{
		_impl->run();
	}
}

void BaseAlgorithm::stop()
{
	if (_impl != nullptr)
	{
		_impl->stop();
	}
}

PositionId BaseAlgorithm::longMarketNoStop(std::string ticker, int numShares)
{
	return _impl->longMarketNoStop(ticker, numShares);
}

PositionId BaseAlgorithm::longMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return _impl->longMarketStopMarket(ticker, numShares, stopPrice);
}

PositionId BaseAlgorithm::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return _impl->longMarketStopLimit(ticker, numShares, activationPrice, limitPrice);
}

PositionId BaseAlgorithm::longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return _impl->longLimitStopMarket(ticker, numShares, buyLimit, activationPrice);
}

PositionId BaseAlgorithm::longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return _impl->longLimitStopLimit(ticker, numShares, buyLimit, activationPrice, limitPrice);
}

PositionId BaseAlgorithm::shortMarketNoStop(std::string ticker, int numShares)
{
	return _impl->shortMarketNoStop(ticker, numShares);
}

PositionId BaseAlgorithm::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return _impl->shortMarketStopMarket(ticker, numShares, activationPrice);
}

PositionId BaseAlgorithm::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return _impl->shortMarketStopLimit(ticker, numShares, activationPrice, limitPrice);
}

PositionId BaseAlgorithm::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return _impl->shortLimitStopMarket(ticker, numShares, buyLimit, activationPrice);
}

PositionId BaseAlgorithm::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return _impl->shortLimitStopLimit(ticker, numShares, buyLimit, activationPrice, limitPrice);
}

void BaseAlgorithm::closePosition(PositionId posId)
{
	_impl->closePosition(posId);
}

void BaseAlgorithm::reducePosition(PositionId posId, int numShares)
{
	_impl->reducePosition(posId, numShares);
}

Position BaseAlgorithm::getPosition(PositionId posId)
{
	return _impl->getPosition(posId);
}
