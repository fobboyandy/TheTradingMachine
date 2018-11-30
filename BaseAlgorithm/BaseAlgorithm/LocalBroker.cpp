#include "LocalBroker.h"

LocalBroker::LocalBroker(std::string input, std::shared_ptr<InteractiveBrokersClient> ibApi, bool live) :
	ibApi_(ibApi),
	tickSource_(input, ibApi),
	liveTrade_(live)
{
	// if live option is turned on but invalid conection is provided
	// then we can't trade live.
	// when trading live, all orders are sent to ib and all positions are
	// maintained by IB. 
	if (live)
	{
		if (ibApi == nullptr || !ibApi->isReady())
		{
			throw std::runtime_error("A valid IB connection is needed for live trading.");
		}
	}
	// for non live orders, we maintain all positions and orders locally.
	// stoplosses are handled by stoplossHandler
	else
	{
		//register stophandler for tick callback from data source.
		stoplossHandlerHandle = tickSource_.registerListener([this](const Tick& tick)
		{
			this->stoplossHandler(tick);
		});
	}
}

LocalBroker::~LocalBroker()
{
	tickSource_.unregisterCallback(stoplossHandlerHandle);
}

void LocalBroker::run()
{
	tickSource_.run();
}

bool LocalBroker::valid()
{
	return valid_;
}

PositionId LocalBroker::longMarketNoStop(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification)
{
	//validate number of shares
	numShares = abs(numShares);

	auto newPosId = portfolio_.newPosition();

	// this lambda captures the state of the current context
	// when this is called as a callback later, it will have
	// the context to dispatch to the caller
	auto fillPositionNotification = [this, newPosId, fillNotification, numShares](double price, time_t time) 
	{
		portfolio_.fillPosition(newPosId, price, numShares, time);
		fillNotification(price, time);
	};

	if (liveTrade_)
	{
		// submit through ib and register fillPosition as the callback
	}
	else
	{
		// for non live trades, we simply fill the position with the latest price
		// instantly and notify the caller. in this case, the caller will be notified
		// before the posId is returned back to them. this is fine because the posId is not
		// given as part of the callback argument anyway and would not provide any useful 
		// information
		fillPositionNotification(tickSource_.lastTick().price, tickSource_.lastTick().time);
	}

	return newPosId;
}

PositionId LocalBroker::longMarketStopMarket(std::string ticker, int numShares, double stopPrice,  std::function<void(double, time_t)> fillNotification)
{
	return PositionId();
}

PositionId LocalBroker::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice,  std::function<void(double, time_t)> fillNotification)
{
	return PositionId();
}

PositionId LocalBroker::longLimitStopMarket(std::string ticker, int numShares, double longLimit, double activationPrice,  std::function<void(double, time_t)> fillNotification)
{
	return PositionId();
}

PositionId LocalBroker::longLimitStopLimit(std::string ticker, int numShares, double longLimit, double activationPrice, double limitPrice,  std::function<void(double, time_t)> fillNotification)
{
	return PositionId();
}

PositionId LocalBroker::shortMarketNoStop(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification)
{
	return 0;
}

PositionId LocalBroker::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice,  std::function<void(double, time_t)> fillNotification)
{
	return PositionId();
}

PositionId LocalBroker::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice,  std::function<void(double, time_t)> fillNotification)
{
	return PositionId();
}

PositionId LocalBroker::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice,  std::function<void(double, time_t)> fillNotification)
{
	return PositionId();
}

PositionId LocalBroker::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice,  std::function<void(double, time_t)> fillNotification)
{
	return PositionId();
}

void LocalBroker::reducePosition(PositionId posId, int numShares)
{
	if (liveTrade_)
	{
		// we are supposed to create an order and submit it to ibapi. ib api
		// returns an orderId to us. We need to map this OrderId to to a position 
		// that the orderUpdate callback from ibapi will be able to update the appropriate
		// position

		// all orders are submitted as all or none

		// skip this part for now. we don't need it until we find a working algo

	}
	//if not live trading, then all market positions are filled instantly at the last seen price
	else
	{
		// autofill
		portfolio_.reducePosition(posId, tickSource_.lastTick().price, numShares);
	}
}

Position LocalBroker::getPosition(PositionId posId)
{
	return portfolio_.getPosition(posId);
}

CallbackHandle LocalBroker::registerListener(TickListener callback)
{
	return tickSource_.registerListener(callback);
}

void LocalBroker::unregisterListener(CallbackHandle handle)
{
	tickSource_.unregisterCallback(handle);
}

void LocalBroker::stoplossHandler(const Tick & tick)
{
}

void LocalBroker::closePosition(PositionId posId, std::function<void(double, time_t)> fillNotification)
{
	// this lambda captures the state of the current context
	// when this is called as a callback later, it will have
	// the context to dispatch to the caller
	auto closePositionFillNotification = [this, posId, fillNotification](double price, time_t time)
	{
		portfolio_.closePosition(posId, price, time);
		fillNotification(price, time);
	};

	if (liveTrade_)
	{
		// we are supposed to create an order and submit it to ibapi. ib api
		// returns an orderId to us. We need to map this OrderId to to a position 
		// that the orderUpdate callback from ibapi will be able to update the appropriate
		// position

		// all orders are submitted as all or none

		// skip this part for now. we don't need it until we find a working algo

	}
	//if not live trading, then all market positions are filled instantly at the last seen price
	else
	{
		// autofill
		closePositionFillNotification(tickSource_.lastTick().price, tickSource_.lastTick().time);
	}
}
