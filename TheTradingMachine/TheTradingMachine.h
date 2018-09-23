#pragma once

#include <thread>
#include <memory>
#include <functional>
#include <atomic>
#include <fstream>
#include <string>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>

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

// plot data structure shared with the gui. plot data is stored in this format.
// the gui is provided an address to the plot data struct and notified upon 
// new data
struct PlotData
{
	std::mutex plotDataMtx;
	bool finished;
	std::queue<Tick> buffer;
	std::vector<Tick> ticks;
	std::vector<std::string> action;
};

class THETRADINGMACHINEDLL IBInterfaceClient
{
public:
	IBInterfaceClient();	
	~IBInterfaceClient();
	void requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback);
	bool isReady(void);

private:
	IBInterface* client;
	std::thread* messageProcess_;
	std::atomic<bool>* runThread;
	void messageProcess(void);
};

class THETRADINGMACHINEDLL TheTradingMachine
{
public:
	explicit TheTradingMachine(std::string in, IBInterfaceClient* ibApiPtr = nullptr);
	virtual ~TheTradingMachine();
	// we need to create a pointer of shared pointer because it is not safe to dll export stl objects
	std::shared_ptr<PlotData>* plotData;
private:
	void preTickHandler(const Tick& tick);
	std::fstream* tickDataFile;
	bool realtime;
	std::string* input;
	IBInterfaceClient* ibapi;
	std::thread* readTickDataThread;
	std::atomic<bool>* runReadTickDataThread;
	void readTickFile(void);

protected:
	void start(void);
	void stop(void);
	virtual void tickHandler(const Tick& tick) = 0;
};

THETRADINGMACHINEDLL IBInterfaceClient* InitializeIbInterfaceClient(void);