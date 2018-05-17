#ifndef stock_h
#define stock_h


#include <string>
#include <vector>

using namespace std;


struct Candlebar
{
	Candlebar();
	Candlebar(double o, double h, double l, double c);
	double open;
	double high;
	double low;
	double close;
	long volume;
	double wap;
	int count;
	bool valid;
};

struct Stock
{
	Stock();
	Stock(const string& t);
	Candlebar getLastCandle(void) const;
	string ticker;
	vector<Candlebar> candlebars;
};

#endif