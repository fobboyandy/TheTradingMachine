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
	_positions[currentPositionId] = Position{0, 0, 0};
	return currentPositionId;
}

// caller handles errorchecking
void Portfolio::fillPosition(PositionId posId, double avgFillPrice, int numShares)
{
	// only update existing positions that hasn't been filled
	if (existingPosition(posId) && _positions[posId].shares == 0)
	{
		_positions[posId].shares = numShares;
		_positions[posId].averagePrice = avgFillPrice;
		_positions[posId].profit = 0;
	}
}

void Portfolio::closePosition(PositionId posId, double avgFillPrice)
{
	if (existingPosition(posId))
	{
		reducePosition(posId, avgFillPrice, _positions[posId].shares);
	}
}

void Portfolio::reducePosition(PositionId posId, double avgFillPrice, int numShares)
{
	if (existingPosition(posId) && _positions[posId].shares != 0)
	{
		// deal with positive numbers
		numShares = abs(numShares);

		// existing position is a long
		if (_positions[posId].shares > 0)
		{
			// profit positive for rising price.
			_positions[posId].profit += (avgFillPrice - _positions[posId].averagePrice) * numShares;
		}
		else
		{
			// profit is positive  for falling price
			_positions[posId].profit += -(avgFillPrice - _positions[posId].averagePrice) * numShares;
		}
		_positions[posId].shares -= numShares;
	}
}

Position Portfolio::getPosition(PositionId posId)
{
	return Position();
}

bool Portfolio::existingPosition(PositionId posId)
{
	return (_positions.find(posId) != _positions.end());
}
