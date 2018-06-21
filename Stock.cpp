#include "Stock.h"


Candlebar::Candlebar()
{

}

Candlebar::Candlebar(double o, double h, double l, double c):
	open(o),
	high(h),
	low(l),
	close(c)
{}


Stock::Stock() : historicalDataDone(false)
{

}

Stock::Stock(const string& t) :ticker(t) , historicalDataDone(false)
{}

Stock & Stock::operator=(const Stock & other) 
{
	this->ticker = other.getTicker();
	other.candlebarMutex.lock();
	this->candlebars = other.candlebars;
	other.candlebarMutex.unlock();
	
	return *this;
}

Candlebar Stock::getLastCandle(void) const
{
	Candlebar candle;
	candlebarMutex.lock();
	if(candlebars.size() > 0 )
		candle = candlebars.back();
	candlebarMutex.unlock();
	return candle;
}

Candlebar Stock::getCandle(size_t index) const
{
	candlebarMutex.lock();
	Candlebar candle = candlebars[index];
	candlebarMutex.unlock();
	return candle;
}

const vector<Candlebar>& Stock::getCandlebars() const
{
	// TODO: insert return statement here
	return candlebars;
}

void Stock::addCandlebar(const Candlebar & candle)
{
	candlebarMutex.lock();
	candlebars.push_back(candle);
	candlebarMutex.unlock();
}

string Stock::getTicker() const
{
	return ticker;
}

size_t Stock::getCandlebarCount() const
{
	size_t candlebarCount = 0;
	candlebarMutex.lock();
	candlebarCount = candlebars.size();
	candlebarMutex.unlock();

	return candlebarCount;
}

bool Stock::isHistoricalDataCompleted() const
{
	bool historicalDataDoneRead;

	candlebarMutex.lock();
	historicalDataDoneRead = historicalDataDone;
	candlebarMutex.unlock();

	return historicalDataDoneRead;
}

void Stock::setHistoricalDataComplete()
{
	candlebarMutex.lock();
	historicalDataDone = true;
	candlebarMutex.unlock();
}
