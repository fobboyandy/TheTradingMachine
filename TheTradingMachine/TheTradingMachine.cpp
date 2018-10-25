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
	explicit TheTradingMachineImpl(std::string in, std::function<void(const Tick&)> algTickCallback, std::shared_ptr<IBInterfaceClient> ibApiPtr);
	~TheTradingMachineImpl();
	std::shared_ptr<PlotData> plotData;
	bool valid(void) const;

private:
	enum class Mode
	{
		REALTIME,
		PLAYBACK
	};

	std::shared_ptr<IBInterfaceClient> ibApi;
	TickDataSource tickDataSource;

	std::function<void(const Tick&)> tickHandler;
	void engineTickHandler(const Tick& tick);
	Mode operationMode;
	bool valid_;

//Ordering System 
private:
	std::unique_ptr<OrderSystem> orderSystem;
public: 
	PositionId buyMarketNoStop(std::string ticker);
	PositionId buyMarketStopMarket(std::string ticker, double stopPrice);
	PositionId buyMarketStopLimit(std::string ticker, double activationPrice, double limitPrice);
	PositionId buyLimitStopMarket(std::string ticker, double buyLimit, double activationPrice);
	PositionId buyLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice);

	PositionId sellMarketNoStop(std::string ticker);
	PositionId sellMarketStopMarket(std::string ticker, double activationPrice);
	PositionId sellMarketStopLimit(std::string ticker, double activationPrice, double limitPrice);
	PositionId sellLimitStopMarket(std::string ticker, double buyLimit, double activationPrice);
	PositionId sellLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice);

	// getPosition gets the current state of a position. It returns a copy to the caller.
	Position getPosition(PositionId posId);

	// modifyPosition allows a user to update an existing position if the update is on the same side.
	// for example. a short can only increase or decrease but cannot be turned into a long
	void modifyPosition(PositionId posId, Position newPosition);

	// closes an existing position. It guarantees that an existing position will not be overbought/sold due to a stoploss attached to an order.
	void closePosition(PositionId posId);
};


TheTradingMachine::TheTradingMachineImpl::TheTradingMachineImpl(std::string in, std::function<void(const Tick&)> algTickCallback, std::shared_ptr<IBInterfaceClient> ibApiPtr):
	tickHandler(algTickCallback),
	ibApi(ibApiPtr),
	plotData(std::make_shared<PlotData>()), //plot data must be initialized before preTickHandler is called
	tickDataSource(in, [this](const Tick& tick) { this->engineTickHandler(tick); }, ibApiPtr)
{
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

	valid_ = tickDataSource.valid();
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
	else if (tickDataSource.finished())
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

PositionId TheTradingMachine::TheTradingMachineImpl::buyMarketNoStop(std::string ticker)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::buyMarketStopMarket(std::string ticker, double stopPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::buyMarketStopLimit(std::string ticker, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::buyLimitStopMarket(std::string ticker, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::buyLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::sellMarketNoStop(std::string ticker)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::sellMarketStopMarket(std::string ticker, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::sellMarketStopLimit(std::string ticker, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::sellLimitStopMarket(std::string ticker, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId TheTradingMachine::TheTradingMachineImpl::sellLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice)
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

TheTradingMachine::TheTradingMachine(std::string in, std::function<void(const Tick&)> algTickCallback, std::shared_ptr<IBInterfaceClient> ibApiPtr) :
	impl_(new TheTradingMachineImpl(in, algTickCallback, ibApiPtr))
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

void TheTradingMachine::closePosition(PositionId posId)
{
}
