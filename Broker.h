#pragma once

#include <queue>
#include <vector>
#include <thread>
#include <string>
#include <unordered_map>
#include "Stock.h"

using namespace std;

class Position
{
public:
	Position() {}
	~Position() {}
private:
	string ticker;
	int shares;
	float averagePrice;
};

//broker can be used to retrieve receive streaming data, historical data, and used to place orders;
//when streaming data is requested, a thread is created which continuously places data into a queue
//the broker keeps a map of the ticker name associated with an orderID such that it can route the 
//data stream into the correct queue
class Broker
{
public:
	Broker() {}
	~Broker() {}


private:

	//Maps a ticker symbol to an orderID which is used to index into the activeStocks vector
	unordered_map<string, size_t> stockOrderId;
	vector<Stock> activeStocks;
	vector<thread*> dataStreamThreads;

	//given ticker name, it searches for a valid orderID. This orderID 
	//is used to route the correct data into the appropriate queue, 
	//we need a starts a thread which continuously places data into a queue
	//use something to remember the queue which was started. the destructor will need to 
	//destroy the thread
	void assignDataStream(string tickerName, queue<float>& q) {}

};