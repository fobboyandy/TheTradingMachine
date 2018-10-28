#pragma once

#include <string>
#include "../TheTradingMachine/TheTradingMachine.h"
#include "../TheTradingMachine/Common.h"
#include "../TheTradingMachine/Position.h"

#ifdef BASEALGORITHM_EXPORTS
#define BASEALGORITHM_DLL __declspec(dllexport)
#else
#define BASEALGORITHM_DLL  __declspec(dllimport)
#endif

class BASEALGORITHM_DLL BaseAlgorithm
{
public:
	BaseAlgorithm(std::string ticker, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live = false);
	virtual ~BaseAlgorithm();

	virtual void tickHandler(const Tick& tick) = 0;

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

private:
	class BaseAlgorithmImpl;
	BaseAlgorithmImpl* _impl;

	// allow impl to access tickHandler
	friend BaseAlgorithmImpl;
};