#pragma once

#include <unordered_map>
#include <string>
#include "Common.h"

// Portfolio contains many different trade positions of a SINGLE stock
// Each position is identified by the position id. 
class Portfolio
{
public:
	Portfolio();
	~Portfolio();

	// all orders are submitted as all or none so we either fill the entire position or none
	// therefore, this should only be called once for any order
	void fillPosition(PositionId posId, double avgFillPrice, int numShares);

	// close position reduces all the shares to 0. If it was a long, it sells. If it was a short, it covers.
	void closePosition(PositionId posId, double avgFillPrice);

	// reducePosition reduces the position in the opposite direction. If it was a long, it sells. If it's a short, it covers. 
	// It will never over reduce an existing position. ie. it will never oversell a long position or overcover a short position.
	void reducePosition(PositionId posId, double avgFillPrice, int numShares);

	Position getPosition(PositionId posId);

private:
	std::unordered_map<PositionId, Position> positions_;

//helpers
private:
	bool existingPosition(PositionId posId);
};