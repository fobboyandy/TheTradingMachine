#pragma once

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
	time_t openTime;
	time_t closeTime;
};