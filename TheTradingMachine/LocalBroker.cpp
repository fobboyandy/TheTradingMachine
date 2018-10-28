#include "LocalBroker.h"



LocalBroker::LocalBroker(std::string input) :
	_dataSource(input),
	_liveTrade(false),
	_valid(false)
{
	_valid = _dataSource.valid();
}

LocalBroker::LocalBroker(std::string ticker, std::shared_ptr<IBInterfaceClient> ibApi, bool live) :
	_ibApi(ibApi),
	_dataSource(ticker, ibApi),
	_liveTrade(live),
	_valid(false)
{
	// if live trading, we send all stoploss orders to interactive broker
	// if non live trading, we register stoplossHandler as a callback
	// to locally monitor stoplosses
	if (_dataSource.valid() && ibApi->isReady())
	{
		// for non live trades, we manage stoplosses locally using
		// stoplossHandler
		if (!_liveTrade)
		{
			//register stophandler for tick callback from data source.
			stoplossHandlerHandle = _dataSource.registerCallback([this](const Tick& tick)
			{
				this->stoplossHandler(tick);
			});
		}
		_valid = true;
	}
}

LocalBroker::~LocalBroker()
{
	_dataSource.unregisterCallback(stoplossHandlerHandle);
}

void LocalBroker::run()
{
	_dataSource.run();
}

bool LocalBroker::valid()
{
	return _valid;
}

PositionId LocalBroker::longMarketNoStop(std::string ticker, int numShares)
{
	//validate number of shares
	numShares = abs(numShares);

	PositionId posId;
	if (_liveTrade)
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
		posId = _portfolio.newPosition();
		
		// data source's tick dispatch runs on the same thread as this function
		// so _dataSource's lastPrice is always within sync
		_portfolio.fillPosition(posId, _dataSource.lastPrice(), numShares);
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
	if (_liveTrade)
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
		posId = _portfolio.newPosition();
		// autofill
		// short positions are negative
		_portfolio.fillPosition(posId, _dataSource.lastPrice(), -numShares);
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
	if (_liveTrade)
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
		_portfolio.reducePosition(posId, _dataSource.lastPrice(), numShares);
	}
}

Position LocalBroker::getPosition(PositionId posId)
{
	return Position();
}

CallbackHandle LocalBroker::registerCallback(TickCallbackFunction callback)
{
	return _dataSource.registerCallback(callback);
}

void LocalBroker::unregisterCallback(CallbackHandle handle)
{
	_dataSource.unregisterCallback(handle);
}

void LocalBroker::stoplossHandler(const Tick & tick)
{
}

void LocalBroker::ibOrderUpdate(OrderId oid, Position p)
{
}

void LocalBroker::closePosition(PositionId posId)
{
	if (_liveTrade)
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
		_portfolio.closePosition(posId, _dataSource.lastPrice());
	}
}
