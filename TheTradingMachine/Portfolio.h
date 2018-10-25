#pragma once

#include <unordered_map>
#include <string>
#include "../IBInterfaceClient/IBInterfaceClient.h"

using PositionId = int;

//
// Structure for holding information about a position. This is created initially as an empty
// position. It must be filled before it becomes valid either through the constructor or 
// using fillPosition
//
class Position
{
public:
	Position();
	Position(std::string ticker, int size);
	~Position();

	void fillPosition();
	void fillPosition(int numShares);
private:
	std::string ticker;
	double averageFillPrice;
	int filled;
	int remaining;
};

class Portfolio
{
public:
	Portfolio();
	~Portfolio();

	// calling this returns a unique position id which identifies the position to be
	// added to the portfolio. This function only allocates an empty position to the 
	// portfolio. It should be filled with the number of shares by calling Position::fillPosition(int)
	PositionId newPosition(Position pos);

private:
	PositionId _uniquePositionId;
	std::unordered_map<int, Position> _positions;
};