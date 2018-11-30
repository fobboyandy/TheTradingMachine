#include "Portfolio.h"

Portfolio::Portfolio():
	uniquePositionId_(0)
{
}

Portfolio::~Portfolio()
{
}

PositionId Portfolio::newPosition()
{
	// use postIncrement to keep consistent with m_orderId in ibApi
	auto currentPositionId = uniquePositionId_++;
	positions_[currentPositionId] = Position{ 0 };
	return currentPositionId;
}

// caller handles errorchecking
void Portfolio::fillPosition(PositionId posId, double avgFillPrice, int numShares, time_t fillTime)
{
	// only update existing positions that hasn't been filled
	if (existingPosition(posId) && positions_[posId].shares == 0)
	{
		positions_[posId].shares = numShares;
		positions_[posId].averagePrice = avgFillPrice;
		positions_[posId].profit = 0;
		positions_[posId].openTime = fillTime;
		positions_[posId].closeTime = 0;
	}
}

void Portfolio::closePosition(PositionId posId, double avgFillPrice, time_t closeTime)
{
	if (existingPosition(posId))
	{
		reducePosition(posId, avgFillPrice, positions_[posId].shares);
		positions_[posId].closeTime = closeTime;
	}
}

void Portfolio::reducePosition(PositionId posId, double avgFillPrice, int numShares)
{
	if (existingPosition(posId) && positions_[posId].shares != 0)
	{
		// deal with positive numbers
		numShares = abs(numShares);

		// existing position is a long
		if (positions_[posId].shares > 0)
		{
			// profit positive for rising price.
			positions_[posId].profit += (avgFillPrice - positions_[posId].averagePrice) * numShares;
		}
		else
		{
			// profit is positive  for falling price
			positions_[posId].profit += -(avgFillPrice - positions_[posId].averagePrice) * numShares;
		}
		positions_[posId].shares -= numShares;
	}
}

Position Portfolio::getPosition(PositionId posId)
{
	return positions_[posId];
}

bool Portfolio::existingPosition(PositionId posId)
{
	return (positions_.find(posId) != positions_.end());
}
