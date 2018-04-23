#ifndef stock_h
#define stock_h


#include <string>
#include <vector>

using namespace std;

struct Stock
{
	Stock();
	Stock(const string& t);
	double getLatestPrice(void) const;
	string ticker;
	vector<double> priceStream;
};

#endif