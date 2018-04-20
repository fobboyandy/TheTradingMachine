#pragma once

#include <queue>
#include <vector>
#include <thread>
#include <string>
#include <unordered_map>

using namespace std;
class Stock
{
public:
	Stock();
	Stock(string tickerSymbol);
	Stock(int tickerId);
	~Stock();

	float getCurrentPrice(void);

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
	Broker();
	~Broker();

	//given a tickerName, looks to see if it's already active. if not, create an OrderID, map the 
	//ticker ID to the orderID, create a new Stock in the list of stocks, start a new thread,
	//stream data into stock
	const Stock& getStockData(string ticker);
	const Stock& getStockData(int ticker);

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
	void assignDataStream(string tickerName, queue<float>& q);

};