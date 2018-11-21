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

PositionId LocalBroker::longMarketNoStop(std::string ticker, int numShares)
{
	//validate number of shares
	numShares = abs(numShares);

	PositionId posId;
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
		posId = portfolio_.newPosition();
		
		// data source's tick dispatch runs on the same thread as this function
		// so _dataSource's lastPrice is always within sync
		portfolio_.fillPosition(posId, tickSource_.lastPrice(), numShares);
	}

	return posId;
}

PositionId LocalBroker::longMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return PositionId();
}

PositionId LocalBroker::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId LocalBroker::longLimitStopMarket(std::string ticker, int numShares, double longLimit, double activationPrice)
{
	return PositionId();
}

PositionId LocalBroker::longLimitStopLimit(std::string ticker, int numShares, double longLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId LocalBroker::shortMarketNoStop(std::string ticker, int numShares)
{
	//validate number of shares 
	numShares = abs(numShares);

	PositionId posId;
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
		posId = portfolio_.newPosition();
		// autofill
		// short positions are negative
		portfolio_.fillPosition(posId, tickSource_.lastPrice(), -numShares);
	}

	return posId;
}

PositionId LocalBroker::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId LocalBroker::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId LocalBroker::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId LocalBroker::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
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
		portfolio_.reducePosition(posId, tickSource_.lastPrice(), numShares);
	}
}

Position LocalBroker::getPosition(PositionId posId)
{
	return Position();
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

void LocalBroker::ibOrderNotification(OrderId oid, Position p)
{
}

void LocalBroker::closePosition(PositionId posId)
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
		portfolio_.closePosition(posId, tickSource_.lastPrice());
	}
}
