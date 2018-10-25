#include "Portfolio.h"

Portfolio::Portfolio()
{
}

Portfolio::~Portfolio()
{
}

Portfolio::PositionId Portfolio::newPosition(Position pos)
{
	return PositionId();
}


Position::Position(std::string ticker, int size)
{
}

Position::~Position()
{
}

void Position::fillPosition()
{
}

void Position::fillPosition(int numShares)
{
}
