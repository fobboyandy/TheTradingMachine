#include "Portfolio.h"

Portfolio::Portfolio():
	_uniquePositionId(0)
{
}

Portfolio::~Portfolio()
{
}

PositionId Portfolio::newPosition()
{
	// use postIncrement to keep consistent with m_orderId in ibApi
	auto currentPositionId = _uniquePositionId++;
	_positions[currentPositionId] = Position{0, 0};
	return currentPositionId;
}

// caller handles errorchecking
void Portfolio::fillPosition(PositionId posId, double avgPrice, int numShares)
{
	_positions[posId].shares = numShares;
	_positions[posId].averagePrice = avgPrice;
}