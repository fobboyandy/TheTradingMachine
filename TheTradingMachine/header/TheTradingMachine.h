#pragma once

#include <queue>
#include <vector>

using namespace std;

class TheTradingMachine
{

public:
	TheTradingMachine();
	TheTradingMachine(int dataSize);

private:

protected:
	queue<double> dataSet;

};