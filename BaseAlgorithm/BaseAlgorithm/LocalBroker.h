#pragma once


#include <unordered_map>
#include <string>
#include <memory>
#include "../../InteractiveBrokersClient/InteractiveBrokersApi/CommonDefs.h"
#include "../../InteractiveBrokersClient/InteractiveBrokersClient/InteractiveBrokersClient.h"
#include "Common.h"
#include "Portfolio.h"
#include "TickBroadcast.h"


// Local Broker acts as a basic broker system for the engine. The engine places orders through the local broker.
// If live trading is turned on, local broker routes the orders to interative brokers for real trading. Otherwise,
// it relies on the tick data from TickDataSource to emulate trading positions and stoplosses locally (paper trading).
class LocalBroker
{
public: 
	LocalBroker(std::string input, std::shared_ptr<InteractiveBrokersClient> ibApi, bool live);
	~LocalBroker();

	void run();
	bool valid();
	CallbackHandle registerListener(TickListener callback);
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

	void reducePosition(PositionId posId, int numShares);
	void closePosition(PositionId posId);

	Position getPosition(PositionId posId);
private:
	// handles stoplosses locally without sending a stoploss order to ib. also used
	// for fileplayback stoploss emulation
	CallbackHandle stoplossHandlerHandle;
	void stoplossHandler(const Tick& tick);

	void ibOrderNotification(OrderId oid, Position p);

private:
	Portfolio portfolio_;

	// used to create a mapping from OrderId to PositionId in order to
	// route ibOrderUpdate callback to the proper position using the orderId
	// this is only used in live trading mode.
	std::unordered_map<OrderId, PositionId> orderPositionMap_;
	std::shared_ptr<InteractiveBrokersClient> ibApi_;

	TickBroadcast dataSource_;
	const bool liveTrade_; 
	bool valid_;
};