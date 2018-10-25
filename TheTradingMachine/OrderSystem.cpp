#include "OrderSystem.h"


PositionId OrderSystem::buyMarketNoStop(std::string ticker)
{
	return PositionId();
}

PositionId OrderSystem::buyMarketStopMarket(std::string ticker, double stopPrice)
{
	return PositionId();
}

PositionId OrderSystem::buyMarketStopLimit(std::string ticker, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId OrderSystem::buyLimitStopMarket(std::string ticker, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId OrderSystem::buyLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId OrderSystem::sellMarketNoStop(std::string ticker)
{
	return PositionId();
}

PositionId OrderSystem::sellMarketStopMarket(std::string ticker, double activationPrice)
{
	return PositionId();
}

PositionId OrderSystem::sellMarketStopLimit(std::string ticker, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId OrderSystem::sellLimitStopMarket(std::string ticker, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId OrderSystem::sellLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice)
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

void OrderSystem::closePosition(PositionId posId)
{
}
