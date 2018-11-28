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
	void unregisterListener(CallbackHandle handle);

// order api
public:
	//long orders
	PositionId longMarketNoStop(std::string ticker, int numShares, std::function<void(double avgPrice)> callerFillNotification);
	PositionId longMarketStopMarket(std::string ticker, int numShares, double stopPrice, std::function<void(double avgPrice)> callerFillNotification);
	PositionId longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice, std::function<void(double avgPrice)> callerFillNotification);
	PositionId longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice, std::function<void(double avgPrice)> callerFillNotification);
	PositionId longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice, std::function<void(double avgPrice)> callerFillNotification);

	//short orders
	PositionId shortMarketNoStop(std::string ticker, int numShares, std::function<void(double avgPrice)> callerFillNotification);
	PositionId shortMarketStopMarket(std::string ticker, int numShares, double activationPrice, std::function<void(double avgPrice)> callerFillNotification);
	PositionId shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice, std::function<void(double avgPrice)> callerFillNotification);
	PositionId shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice, std::function<void(double avgPrice)> callerFillNotification);
	PositionId shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice, std::function<void(double avgPrice)> callerFillNotification);

	void reducePosition(PositionId posId, int numShares);
	void closePosition(PositionId posId);

	Position getPosition(PositionId posId);
	void executePosition(PositionId posId, std::function<void(double avgPrice)> callerFillNotification);
private:
	// handles stoplosses locally without sending a stoploss order to ib. also used
	// for fileplayback stoploss emulation
	CallbackHandle stoplossHandlerHandle;
	void stoplossHandler(const Tick& tick);

private:
	// simple orders used to record a position before it's
	// executed
	struct SimpleOrder;

	// keeps track of orders that have been placed but not executed
	// we use pending orders because we want to give the caller a 
	// chance to set up their order notification callback with the
	// position id. If the order was filled before the ordering function
	// returned, the callers notification would be called without 
	// knowing which position has been filled. once they have set up
	// their notification functions with the returned position id, they 
	// can execute the order.
	PositionId uniquePendingOrderIds_;
	std::unordered_map<PositionId, std::unique_ptr<SimpleOrder>> pendingOrders_;

	std::shared_ptr<InteractiveBrokersClient> ibApi_;
	TickBroadcast tickSource_;

	Portfolio portfolio_;
	const bool liveTrade_; 
	bool valid_;
};

struct LocalBroker::SimpleOrder
{
	enum class OrderType
	{
		LONGMARKETNOSTOP,
		LONGMARKETSTOPMARKET,
		LONGMARKETSTOPLIMIT,
		LONGLIMITSTOPMARKET,
		LONGLIMITSTOPLIMIT,

		SHORTMARKETNOSTOP,
		SHORTMARKETSTOPMARKET,
		SHORTMARKETSTOPLIMIT,
		SHORTLIMITSTOPMARKET,
		SHORTLIMITSTOPLIMIT
	};

	OrderType type;
	double entryLimit;
	double stopLimit;
	int shares;

};