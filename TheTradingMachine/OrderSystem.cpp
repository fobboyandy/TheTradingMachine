#include "OrderSystem.h"


OrderSystem::OrderSystem(std::shared_ptr<IBInterfaceClient> ibApi):
	_OrderPosition(std::make_unique<std::unordered_map<OrderId, PositionId>>()),
	_ibApi(ibApi),
	_liveTrade(true)
{

}

OrderSystem::OrderSystem(std::shared_ptr<TickDataSource> dataSource) :
	_dataSource(dataSource),
	_liveTrade(false)
{

}

OrderSystem::~OrderSystem()
{
}

PositionId OrderSystem::buyMarketNoStop(std::string ticker, int numShares)
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
	//if not live trading, then all market positions are filled instantly at the last price
	else
	{
		posId = _portfolio.newPosition();
		// autofill 
		_portfolio.fillPosition(posId, _dataSource->lastPrice(), numShares);
	}

	return posId;
}

PositionId OrderSystem::buyMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return PositionId();
}

PositionId OrderSystem::buyMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId OrderSystem::buyLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId OrderSystem::buyLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId OrderSystem::sellMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
}

PositionId OrderSystem::sellMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId OrderSystem::sellMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId OrderSystem::sellLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId OrderSystem::sellLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

Position OrderSystem::getPosition(PositionId posId)
{
	return Position();
}

void OrderSystem::modifyPosition(PositionId posId, Position newPosition)
{
}

void OrderSystem::ibOrderUpdate(OrderId oid, Position p)
{
}

void OrderSystem::closePosition(PositionId posId)
{
}
