#pragma once

#include <unordered_map>
#include <string>
#include "../IBInterfaceClient/IBInterfaceClient.h"
#include "Common.h"

//
// Structure for holding information about a position. This is created initially as an empty
// position. It is initialized with no shares. Because we only do all or none orders, when an order
// is filled, shares will hold the number of shares that we hold. if shares == 0, then we have no shares
//
struct Position
{
	double averagePrice;
	int shares;
	double profit;
};

// Portfolio contains many different trade positions of a SINGLE stock
// Each position is identified by the position id. 
class Portfolio
{
public:
	Portfolio();
	~Portfolio();

	// calling this returns a unique position id which identifies the position to be
	// added to the portfolio. This function only allocates an empty position to the 
	// portfolio. It should be filled with the number of shares by calling Position::fillPosition(int)
	PositionId newPosition();

	// all orders are submitted as all or none so we either fill the entire position or none
	// therefore, this should only be called once for any order
	void fillPosition(PositionId posId, double avgPrice, int numShares);

private:
	PositionId _uniquePositionId;
	std::unordered_map<PositionId, Position> _positions;
};