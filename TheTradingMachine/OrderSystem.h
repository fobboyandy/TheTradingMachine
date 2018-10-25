#pragma once


#include <unordered_map>
#include "../IBInterface/CommonDefs.h"
#include "../IBInterfaceClient/IBInterfaceClient.h"
#include "Portfolio.h"

class OrderSystem
{
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
	void closePosition(PositionId posId);
	Position getPosition(PositionId posId);
	void modifyPosition(PositionId posId, Position newPosition);

	
private:

	// used to create a mapping from OrderId to PositionId in order to
	// route positionUpdate callback to the proper position using the orderId
	std::unordered_map<OrderId, PositionId> OrderPosition;
};