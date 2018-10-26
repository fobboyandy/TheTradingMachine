#include <iostream>
#include <functional>
#include <atomic>
#include <sstream>
#include <fstream>
#include <thread>
#include <assert.h>
#include "TheTradingMachine.h"
#include "OrderSystem.h"
#include "TickDataSource.h"


class TheTradingMachine::TheTradingMachineImpl
{
public:
	explicit TheTradingMachineImpl(std::string in, std::function<void(const Tick&)> algTickCallback, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live);
	~TheTradingMachineImpl();
	std::shared_ptr<PlotData> plotData;
	bool valid(void) const;

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

	// getPosition gets the current state of a position. It returns a copy to the caller.
	Position getPosition(PositionId posId);

	// modifyPosition allows a user to update an existing position if the update is on the same side.
	// for example. a short can only increase or decrease but cannot be turned into a long
	void modifyPosition(PositionId posId, Position newPosition);

	// closes an existing position. It guarantees that an existing position will not be overbought/sold due to a stoploss attached to an order.
	void closePosition(PositionId posId);

private:
	enum class Mode
	{
		REALTIME,
		PLAYBACK
	};

	std::shared_ptr<IBInterfaceClient> ibApi;
	std::function<void(const Tick&)> tickHandler;
	void engineTickHandler(const Tick& tick);

	// tickDataSource contains a thread which relies engineTickHandler which calls tickHandler.
	// Because destruction happens in reverse order, we need to make sure the thread is stopped
	// before tickHandler is destroyed or else tickHandler may be called by tickDataSource's thread
	// after tickHandler is destroyed
	//tickDataSource is shared with accessee by orderSystem
	std::shared_ptr<TickDataSource> tickDataSource;

	Mode operationMode;
	bool valid_;

//Ordering System 
private:
	//declare as a pointer for lazy initialization
	std::unique_ptr<OrderSystem> orderSystem;
};

TheTradingMachine::TheTradingMachineImpl::TheTradingMachineImpl(std::string in, std::function<void(const Tick&)> algTickCallback, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live) :
	tickHandler(algTickCallback),
	ibApi(ibApiPtr),
	plotData(std::make_shared<PlotData>()), //plot data must be initialized before preTickHandler is called
	tickDataSource(std::make_shared<TickDataSource>(in, ibApiPtr))
{
	//we don't care about the handle because we won't be unregistering this callback
	tickDataSource->registerCallback([this](const Tick& tick) 
	{
		this->engineTickHandler(tick); 
	});

	//
	// Check if it's a recorded data input for backtesting
	//
	if (in.find(".tickdat") != std::string::npos)
	{
		operationMode = Mode::PLAYBACK;
	}
	else if (ibApi != nullptr && ibApi->isReady())
	{
		operationMode = Mode::REALTIME;
	}

	//if live trading is turned on, OrderSystem will offload orders to IB.
	if(live && operationMode == Mode::REALTIME)
	{
		orderSystem = std::make_unique<OrderSystem>(tickDataSource, ibApi, true);
	}
	else
	{
		orderSystem = std::make_unique<OrderSystem>(tickDataSource);
	}

	// all the necessary callbacks have been registered at this point. 
	// start the data source.
	tickDataSource->start();
	if (!tickDataSource->valid())
	{
		throw "Invalid data source!";
	}
	valid_ = true;
}

TheTradingMachine::TheTradingMachineImpl::~TheTradingMachineImpl()
{
}

bool TheTradingMachine::TheTradingMachineImpl::valid(void) const
{
	return valid_;
}

void TheTradingMachine::TheTradingMachineImpl::engineTickHandler(const Tick & tick)
{
	//let algorithm act on the data first
	tickHandler(tick);

	// in case if gui is sometimes slow and is holding the lock, we don't want to block this thread
	// therefore, if we fail to retrieve the lock immediately, we push the data into a secondary buffer.
	// when we successfully get the lock, we simply have to move the data into plotData. this will allow
	// the algorithm to be more overall responsive .
	std::unique_lock<std::mutex> lock(plotData->plotDataMtx, std::defer_lock);
	PlotData& plotDataRef = *plotData;
	//this is the only thread that accesses buffer no synchronization needed
	plotDataRef.buffer.push(tick);
	// if it was able to lock the lock, then unload everything from the buffer into the plotData
	// the buffer should usually. we use trylock in order to prevent GUI operations from blocking 
	// the actual algorithm
	if (lock.try_lock())
	{
		while (!plotDataRef.buffer.empty())
		{
			plotDataRef.ticks.push_back(plotDataRef.buffer.front());
			plotDataRef.buffer.pop();
		}
		lock.unlock();
	}
	// if the stream is finished, we MUST unload the data from the buffer. we have to block here
	// since this is the last time this function will be called from the dataSource
	else if (tickDataSource->finished())
	{
		lock.lock();
		while (!plotData->buffer.empty())
		{
			plotData->ticks.push_back(plotData->buffer.front());
			plotData->buffer.pop();
		}
		plotData->finished = true;
	}

}

PositionId TheTradingMachine::TheTradingMachineImpl::buyMarketNoStop(std::string ticker, int numShares)
{
	auto posId = orderSystem->buyMarketNoStop(ticker, numShares);
	return posId;
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

void TheTradingMachine::TheTradingMachineImpl::closePosition(PositionId posId)
{
}

Position TheTradingMachine::TheTradingMachineImpl::getPosition(PositionId posId)
{
	return Position();
}

void TheTradingMachine::TheTradingMachineImpl::modifyPosition(PositionId posId, Position newPosition)
{
}

TheTradingMachine::TheTradingMachine
(
	std::string in, 
	std::function<void(const Tick&)> algTickCallback, 
	std::shared_ptr<IBInterfaceClient> ibApiPtr, 
	bool live
) :
	impl_(new TheTradingMachineImpl(in, algTickCallback, ibApiPtr, live))
{
}


TheTradingMachine::~TheTradingMachine()
{
	if (impl_ != nullptr)
	{
		delete impl_;
		impl_ = nullptr;
	}
}

std::shared_ptr<PlotData> TheTradingMachine::getPlotData()
{
	return impl_->plotData;
}

bool TheTradingMachine::valid() const
{
	if (impl_ == nullptr)
		return false;
	return impl_->valid();
}

void TheTradingMachine::modifyPosition(PositionId posId, Position newPosition)
{
}

void TheTradingMachine::closePosition(PositionId posId)
{
}

PositionId TheTradingMachine::buyMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
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

Position TheTradingMachine::getPosition(PositionId posId)
{
	return Position();
}
