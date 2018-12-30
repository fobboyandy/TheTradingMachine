#include "LocalBroker.h"
#include <algorithm>

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
}

LocalBroker::~LocalBroker()
{
}

void LocalBroker::run()
{
	tickSource_.run();
}

bool LocalBroker::valid()
{
	return valid_;
}

PositionId LocalBroker::longMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification)
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
		ibApi_->longMarket(ticker, numShares, fillPositionNotification);
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

PositionId LocalBroker::longLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification)
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
		ibApi_->longLimit(ticker, limitPrice, numShares, fillPositionNotification);
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

PositionId LocalBroker::shortMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification)
{
	//validate number of shares
	numShares = abs(numShares);

	auto newPosId = portfolio_.newPosition();

	// this lambda captures the state of the current context
	// when this is called as a callback later, it will have
	// the context to dispatch to the caller
	auto fillPositionNotification = [=](double price, time_t time)
	{
		portfolio_.fillPosition(newPosId, price, -numShares, time);
		fillNotification(price, time);
	};

	if (liveTrade_)
	{
		// submit through ib and register fillPosition as the callback
		ibApi_->shortMarket(ticker, numShares, fillPositionNotification);
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

PositionId LocalBroker::shortLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification)
{
	//validate number of shares
	numShares = abs(numShares);

	auto newPosId = portfolio_.newPosition();

	// this lambda captures the state of the current context
	// when this is called as a callback later, it will have
	// the context to dispatch to the caller
	auto fillPositionNotification = [=](double price, time_t time)
	{
		portfolio_.fillPosition(newPosId, price, -numShares, time);
		fillNotification(price, time);
	};

	if (liveTrade_)
	{
		// submit through ib and register fillPosition as the callback
		ibApi_->shortLimit(ticker, limitPrice, numShares, fillPositionNotification);
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

void LocalBroker::reducePosition(PositionId posId, int numShares, std::function<void(double, time_t)> fillNotification)
{
	// this lambda captures the state of the current context
	// when this is called as a callback later, it will have
	// the context to dispatch to the caller
	// we can simply use numShares as fill amount since we
	// only submit orders as all or none
	auto reducePositionFillNotification = [this, posId, fillNotification, numShares](double price, time_t time)
	{
		portfolio_.reducePosition(posId, price, numShares);
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
		reducePositionFillNotification(tickSource_.lastTick().price, tickSource_.lastTick().time);
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
