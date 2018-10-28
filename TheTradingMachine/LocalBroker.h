#pragma once


#include <unordered_map>
#include <string>
#include <memory>
#include "../IBInterface/CommonDefs.h"
#include "../IBInterfaceClient/IBInterfaceClient.h"
#include "Common.h"
#include "Portfolio.h"
#include "TickDataSource.h"


// Local Broker acts as a basic broker system for the engine. The engine places orders through the local broker.
// If live trading is turned on, local broker routes the orders to interative brokers for real trading. Otherwise,
// it relies on the tick data from TickDataSource to emulate trading positions and stoplosses locally (paper trading).
class LocalBroker
{
public:
	// if only an input is given, then local broker simulates a broker by loading data stored in the file 
	// given in the input string. In this mode, LocalBroker will also fill all orders as paper trades. All
	// orders will be guaranteed to fill instantly. All stoplosses are also maintained locally.
	LocalBroker(std::string input);

	// If a ticker and an active api connection is given, then tick data will come from the broker.
	// By default, live trading will be off and all orders will be maintained locally as paper trades. 
	// If live is turned on, all orders are sent to interactive broker as live orders. all stoplosses
	// are submitted as orders and maintained by interactive broker. orders are not guaranteed to fill
	// in live orders. 
	LocalBroker(std::string ticker, std::shared_ptr<IBInterfaceClient> ibApi, bool live = false);
	~LocalBroker();

	void run();
	bool valid();
	CallbackHandle registerCallback(TickCallbackFunction callback);
	void unregisterCallback(CallbackHandle handle);

// order api
public:

	//long orders
	PositionId longMarketNoStop(std::string ticker, int numShares);
	PositionId longMarketStopMarket(std::string ticker, int numShares, double stopPrice);
	PositionId longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	//short orders
	PositionId shortMarketNoStop(std::string ticker, int numShares);
	PositionId shortMarketStopMarket(std::string ticker, int numShares, double activationPrice);
	PositionId shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	//Profit taking functions
	//close position reduces all the shares to 0. If it was a long, it sells. If it was a short, it covers.
	void closePosition(PositionId posId);

	// reducePosition reduces the position in the opposite direction. If it was a long, it sells. If it's a short, it covers. 
	// It will never over reduce an existing position. ie. it will never oversell a long position or overcover a short position.
	void reducePosition(PositionId posId, int numShares);

	Position getPosition(PositionId posId);

private:
	// handles stoplosses locally without sending a stoploss order to ib. also used
	// for fileplayback stoploss emulation
	CallbackHandle stoplossHandlerHandle;
	void stoplossHandler(const Tick& tick);
	void ibOrderUpdate(OrderId oid, Position p);

private:
	Portfolio _portfolio;

	// used to create a mapping from OrderId to PositionId in order to
	// route ibOrderUpdate callback to the proper position using the orderId
	// this is only used in live trading mode.
	std::unordered_map<OrderId, PositionId> _orderPositionMap;
	std::shared_ptr<IBInterfaceClient> _ibApi;

	TickDataSource _dataSource;
	const bool _liveTrade; 
	bool _valid;
};