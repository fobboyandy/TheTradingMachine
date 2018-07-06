#ifndef stock_h
#define stock_h


#include <string>
#include <vector>
#include <mutex>

using namespace std;


struct Candlebar
{
	Candlebar();
	Candlebar(double o, double h, double l, double c);
	string time; //marks the time of the candle that it was recorded at
	string strength; //support or resistance candle 
	double open;
	double high;
	double low;
	double close;
	long volume;
	double wap;
	int count;
	bool valid;
};

class Stock
{
public:
	Stock();
	Stock(const string& t);
	Stock& operator=(const Stock& other);
	Candlebar getLastCandle() const;
	Candlebar getCandle(size_t index) const;
	const vector<Candlebar>& getCandlebars() const;
	void addCandlebar(const Candlebar& candle);
	string getTicker() const;
	size_t getCandlebarCount() const;

	void setHistoricalDataComplete();

	bool isHistoricalDataCompleted() const;
private:
	int timeFrame;
	vector<Candlebar> candlebars;
	bool historicalDataDone;
	string ticker;
	mutable mutex candlebarMutex;

};

#endif