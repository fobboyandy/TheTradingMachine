#pragma once

#include <thread>
#include <memory>
#include <functional>
#include <atomic>
#include <fstream>
#include <string>

#ifdef EXPORTTHETRADINGMACHINEDLL
#define THETRADINGMACHINEDLL __declspec(dllexport)
#include "../IBInterface/IBInterface.h"
#else
#define THETRADINGMACHINEDLL __declspec(dllimport)
//
// Give derived classes access to basic structures
//
struct TickAttrib
{
	bool canAutoExecute;
	bool pastLimit;
	bool preOpen;
	bool unreported;
	bool bidPastLow;
	bool askPastHigh;
};

struct Tick
{
	int tickType;
	time_t time;
	double price;
	int size;
	TickAttrib attributes;
	std::string exchange;
};

struct Bar
{
	std::string time;
	double high;
	double low;
	double open;
	double close;
	double wap;
	long long volume;
	int count;
};

class IBInterface; 
class IBInterfaceClient;
class TheTradingMachine;
#endif

class THETRADINGMACHINEDLL IBInterfaceClient
{
public:
	IBInterfaceClient();
	~IBInterfaceClient();
	void requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback);

private:
	IBInterface* client;
	std::thread* messageProcess_;
	std::atomic<bool>* runThread;
	void messageProcess(void);
};

class THETRADINGMACHINEDLL TheTradingMachine
{
public:
	explicit TheTradingMachine(std::string input, IBInterfaceClient* ibApiPtr = nullptr);
	virtual ~TheTradingMachine();
	void requestTicks(std::function<void(const Tick& tick)> callback);
private:
	std::fstream* tickDataFile;
	bool realtime;
	std::string* ticker;
	IBInterfaceClient* ibapi;
	std::thread* readTickDataThread;
	std::atomic<bool>* runReadTickDataThread;
	void readTickFile(std::function<void(const Tick& tick)> callback);
};

THETRADINGMACHINEDLL IBInterfaceClient* InitializeIbInterfaceClient(void);