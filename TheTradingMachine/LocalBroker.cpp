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
	else
	{
		throw "Invalid data source.";
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

PositionId LocalBroker::buyMarketNoStop(std::string ticker, int numShares)
{
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
		_portfolio.fillPosition(posId, _dataSource.lastPrice(), numShares);
	}

	return posId;
}

PositionId LocalBroker::buyMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return PositionId();
}

PositionId LocalBroker::buyMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId LocalBroker::buyLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId LocalBroker::buyLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId LocalBroker::sellMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
}

PositionId LocalBroker::sellMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId LocalBroker::sellMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId LocalBroker::sellLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId LocalBroker::sellLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

Position LocalBroker::getPosition(PositionId posId)
{
	return Position();
}

void LocalBroker::modifyPosition(PositionId posId, Position newPosition)
{
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
}
