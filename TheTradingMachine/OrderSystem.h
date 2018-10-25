#pragma once


#include <unordered_map>
#include "../IBInterface/CommonDefs.h"
#include "../IBInterfaceClient/IBInterfaceClient.h"
#include "Portfolio.h"
#include "TickDataSource.h"

class OrderSystem
{
public:
	// two options. construction providing ibApi means placing live orders. otherwise, 
	// providing dataSource means prices are simulated and orders are filled without placing
	// actual orders
	OrderSystem(std::shared_ptr<IBInterfaceClient> ibApi);
	OrderSystem(std::shared_ptr<TickDataSource> dataSource);
	~OrderSystem();
	
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
	void closePosition(PositionId posId);
	Position getPosition(PositionId posId);
	void modifyPosition(PositionId posId, Position newPosition);

private:
	Portfolio _portfolio;

	// used to create a mapping from OrderId to PositionId in order to
	// route ibOrderUpdate callback to the proper position using the orderId
	// we make this a pointer since it only gets instantiated if we were running live trades.
	std::unique_ptr<std::unordered_map<OrderId, PositionId>> _OrderPosition;
	std::shared_ptr<IBInterfaceClient> _ibApi;
	std::shared_ptr<TickDataSource> _dataSource;
	bool _liveTrade; 

	void ibOrderUpdate(OrderId oid, Position p);
};