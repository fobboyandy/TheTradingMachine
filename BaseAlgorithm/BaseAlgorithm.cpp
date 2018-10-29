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

private:
	BaseAlgorithm* parent;
	LocalBroker localBroker;

	CallbackHandle callbackHandle;
	void tickHandler(const Tick& tick);
};

BaseAlgorithm::BaseAlgorithmImpl::BaseAlgorithmImpl(BaseAlgorithm* parentIn, std::string input, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live) :
	parent(parentIn),
	plotData(std::make_shared<PlotData>()),
	localBroker(input, ibApiPtr, live)
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
	localBroker.run();
}

void BaseAlgorithm::BaseAlgorithmImpl::stop()
{
	//unregistering the callback stops the algorithm
	localBroker.unregisterCallback(callbackHandle);
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
	return PositionId();
}

PositionId BaseAlgorithm::longMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::shortMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
}

PositionId BaseAlgorithm::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

void BaseAlgorithm::closePosition(PositionId posId)
{
}

void BaseAlgorithm::reducePosition(PositionId posId, int numShares)
{
}

Position BaseAlgorithm::getPosition(PositionId posId)
{
	return Position();
}
