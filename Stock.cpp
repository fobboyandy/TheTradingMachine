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


Stock::Stock()
{

}

Stock::Stock(const string& t):ticker(t)
{}

Candlebar Stock::getLastCandle(void) const
{
	Candlebar candle;
	candle.valid = false;
	if(candlebars.size() > 0 )
		candle = candlebars[candlebars.size() - 1];
	return candle;
}
