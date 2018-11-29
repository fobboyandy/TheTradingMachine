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
	PositionId longMarketNoStop(std::string ticker, int numShares, std::function<void(double avgPrice)> fillNotification);
	PositionId longMarketStopMarket(std::string ticker, int numShares, double stopPrice, std::function<void(double avgPrice)> fillNotification);
	PositionId longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice, std::function<void(double avgPrice)> fillNotification);
	PositionId longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice, std::function<void(double avgPrice)> fillNotification);
	PositionId longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice, std::function<void(double avgPrice)> fillNotification);

	//short orders
	PositionId shortMarketNoStop(std::string ticker, int numShares, std::function<void(double avgPrice)> fillNotification);
	PositionId shortMarketStopMarket(std::string ticker, int numShares, double activationPrice, std::function<void(double avgPrice)> fillNotification);
	PositionId shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice, std::function<void(double avgPrice)> fillNotification);
	PositionId shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice, std::function<void(double avgPrice)> fillNotification);
	PositionId shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice, std::function<void(double avgPrice)> fillNotification);

	void closePosition(PositionId posId, std::function<void(double avgPrice)> fillNotification);

	Position getPosition(PositionId posId);
private:
	// handles stoplosses locally without sending a stoploss order to ib. also used
	// for fileplayback stoploss emulation
	CallbackHandle stoplossHandlerHandle;
	void stoplossHandler(const Tick& tick);
	void reducePosition(PositionId posId, int numShares);

private:
	std::shared_ptr<InteractiveBrokersClient> ibApi_;
	TickBroadcast tickSource_;

	Portfolio portfolio_;
	const bool liveTrade_; 
	bool valid_;
};