#pragma once
#include <string>
#include "TickAttrib.h"

struct Tick
{
	int tickType;
	time_t time;
	double price;
	int size;
	TickAttrib attributes;
	std::string exchange;
};