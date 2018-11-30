#pragma once

#include <map>
#include <string>
#include <memory>
#include "../InteractiveBrokersApi/CommonDefs.h"
#include "../InteractiveBrokersClient/InteractiveBrokersClient.h"
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
	//basic order api. Parent will use these to implement their own local stoploss handler. This provides more flexibility
	PositionId longMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification);
	PositionId longMarketLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification);
	PositionId shortMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification);
	PositionId shortMarketLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification);
	void closePosition(PositionId posId, std::function<void(double, time_t)> fillNotification);
	Position getPosition(PositionId posId);

private:
	void reducePosition(PositionId posId, int numShares);

private:
	std::shared_ptr<InteractiveBrokersClient> ibApi_;
	TickBroadcast tickSource_;

	Portfolio portfolio_;
	const bool liveTrade_; 
	bool valid_;
};