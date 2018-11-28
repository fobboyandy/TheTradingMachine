#include "Portfolio.h"

Portfolio::Portfolio()
{
}

Portfolio::~Portfolio()
{
}

// caller handles errorchecking
void Portfolio::fillPosition(PositionId posId, double avgFillPrice, int numShares)
{
	// only update existing positions that hasn't been filled
	if (existingPosition(posId) && positions_[posId].shares == 0)
	{
		positions_[posId].shares = numShares;
		positions_[posId].averagePrice = avgFillPrice;
		positions_[posId].profit = 0;
	}
}

void Portfolio::closePosition(PositionId posId, double avgFillPrice)
{
	if (existingPosition(posId))
	{
		reducePosition(posId, avgFillPrice, positions_[posId].shares);
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
	return Position();
}

bool Portfolio::existingPosition(PositionId posId)
{
	return (positions_.find(posId) != positions_.end());
}
