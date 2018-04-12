#pragma once

#include <queue>
#include <vector>
#include <thread>
#include <string>

using namespace std;
class Stock
{
public:
	Stock();
	Stock(string tickerSymbol);
	Stock(int tickerId);
	virtual ~Stock();
	bool startDataStream(void);
private:
	string ticker;
	queue<float> dataStream;
	vector<float> historicalData;
};

class Position
{
public:
	Position();
	~Position();
private:
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
	Broker();
	~Broker();
	//look for the next unused order ID
	void getValidOrderID(void);

	//given ticker name, it searches for a valid orderID. This orderID 
	//is used to route the correct data into the appropriate queue, 
	//we need a starts a thread which continuously places data into a queue
	//use something to remember the queue which was started. the destructor will need to 
	//destroy the thread
	void assignDataStream(string tickerName, queue<float>& q);

private:
	//Given the orderId as index, will retrieve the stream of prices
	std::vector<int> prices;
	std::vector<thread*> dataStreamThreads;
};