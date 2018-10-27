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

// order api
public:
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

	CallbackHandle registerCallback(TickCallbackFunction callback);
	void unregisterCallback(CallbackHandle handle);

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