#pragma once

#include <string>
#include <stdexcept>
#include "Common.h"
#include "PlotData.h"
#include "../IBInterfaceClient/IBInterfaceClient.h"

#ifdef EXPORTTHETRADINGMACHINEDLL
#define THETRADINGMACHINEDLL __declspec(dllexport)
#else
#define THETRADINGMACHINEDLL __declspec(dllimport)
#endif

class THETRADINGMACHINEDLL TheTradingMachine
{
public:
	TheTradingMachine(std::string tickDataFile);
	TheTradingMachine(std::string ticker, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live = false);

	~TheTradingMachine();
	std::shared_ptr<PlotData> getPlotData();

	void setCallback(TickCallbackFunction callback);
	void run();
private:
	class TheTradingMachineImpl; 
	TheTradingMachineImpl* _impl;

// Order api
public:
	// When these order functions are called, a PositionId is returned immediately. The functions
	// do not block until the positions are filled. Since it's non blocking, the position is not guaranteed
	// to be filled when the function returns. With the positionId however, the caller can query the status of the position
	// using getPosition(PositionId).
		
	// all orders are submitted as all or none
	PositionId longMarketNoStop(std::string ticker, int numShares);
	PositionId longMarketStopMarket(std::string ticker, int numShares, double stopPrice);
	PositionId longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	PositionId shortMarketNoStop(std::string ticker, int numShares);
	PositionId shortMarketStopMarket(std::string ticker, int numShares, double activationPrice);
	PositionId shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	void closePosition(PositionId posId);
	void reducePosition(PositionId posId, int numShares);
	Position getPosition(PositionId posId);
};