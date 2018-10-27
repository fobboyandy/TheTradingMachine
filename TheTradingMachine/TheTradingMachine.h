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
};