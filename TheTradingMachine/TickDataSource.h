#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include "../IBInterfaceClient/IBInterfaceClient.h"

class TickDataSource
{
public:
	TickDataSource(std::string in, std::function<void(const Tick&)> callback, std::shared_ptr<IBInterfaceClient> ibApiPtr = nullptr);
	~TickDataSource();
	bool valid() const;
	bool finished() const;
	double lastPrice() const;

private:
	void preTickDispatch(const Tick& tick);
	void readTickFile(void);

private:
	std::function<void(const Tick& tick)> tickDataDispatchCallback;
	std::string input;
	std::shared_ptr<IBInterfaceClient> ibApi;
	int dataStreamHandle;
	std::atomic<bool> threadCancellationToken;
	std::thread readTickDataThread;
	int streamingDataHandle; // when we request real time data, we are given a handle so that we can cancel it upon closing
	bool _valid;
	bool _finished; //finished doesn't need to be atomic since the tickHandler thread runs on the same thread as readTickFile thread
	// price is updated from another thread. make atomic
	std::atomic<double> _lastPrice;
};