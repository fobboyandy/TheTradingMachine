#include "BaseAlgorithm.h"
// BaseAlgorithm.cpp : Defines the exported functions for the DLL application.
//

class BaseAlgorithm::BaseAlgorithmImpl
{
public:
	BaseAlgorithmImpl(BaseAlgorithm* parent, std::string ticker, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live = false);
	~BaseAlgorithmImpl();

public:
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

	void run();
private:

	// we make this a unique_ptr so we can dynamically construct
	// engine based on the constructor of BaseAlgorithmImpl inputs
	std::unique_ptr<TheTradingMachine> engine;
};

BaseAlgorithm::BaseAlgorithm(std::string ticker, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live)
{
}

BaseAlgorithm::~BaseAlgorithm()
{
}

PositionId BaseAlgorithm::longMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
}

PositionId BaseAlgorithm::longMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::shortMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
}

PositionId BaseAlgorithm::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

void BaseAlgorithm::closePosition(PositionId posId)
{
}

void BaseAlgorithm::reducePosition(PositionId posId, int numShares)
{
}

Position BaseAlgorithm::getPosition(PositionId posId)
{
	return Position();
}

void BaseAlgorithm::run()
{
}

BaseAlgorithm::BaseAlgorithmImpl::BaseAlgorithmImpl(BaseAlgorithm * parent, std::string ticker, std::shared_ptr<IBInterfaceClient> ibApiPtr, bool live)
{
}

BaseAlgorithm::BaseAlgorithmImpl::~BaseAlgorithmImpl()
{
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortMarketNoStop(std::string ticker, int numShares)
{
	return PositionId();
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return PositionId();
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return PositionId();
}

void BaseAlgorithm::BaseAlgorithmImpl::closePosition(PositionId posId)
{
}

void BaseAlgorithm::BaseAlgorithmImpl::reducePosition(PositionId posId, int numShares)
{
}

Position BaseAlgorithm::BaseAlgorithmImpl::getPosition(PositionId posId)
{
	return Position();
}

void BaseAlgorithm::BaseAlgorithmImpl::run()
{
}
