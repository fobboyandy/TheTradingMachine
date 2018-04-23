#include "Stock.h"

Stock::Stock()
{

}

Stock::Stock(const string& t):ticker(t)
{}


double Stock::getLatestPrice(void) const
{
	if(priceStream.size() > 1)
		return priceStream[priceStream.size() - 1];
	return -1;
}