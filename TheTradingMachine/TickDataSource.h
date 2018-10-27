#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include "../IBInterfaceClient/IBInterfaceClient.h"


class TickDataSource
{
public:
	TickDataSource(std::string in, std::shared_ptr<IBInterfaceClient> ibApiPtr = nullptr);
	~TickDataSource();
	int registerCallback(std::function<void(const Tick& tick)> callback);
	void unregisterCallback(int handle);
	bool valid() const;
	bool finished() const;
	double lastPrice() const;

	// since callbacks are not registered at constructor, we need to start the file reading
	// thread after the caller has properly registered for their callbacks before starting
	void start();
	void stop();

private:
	void preTickDispatch(const Tick& tick);
	void readTickFile(void);
	
private:	
	// price is updated from another thread. make atomic
	std::atomic<double> _lastPrice;

	std::mutex callbackListMtx;
	int uniqueCallbackHandles;
	std::unordered_map<int, std::function<void(const Tick& tick)>> callbackList;

	std::string input;

	// api data
	std::shared_ptr<IBInterfaceClient> ibApi;
	int dataStreamHandle; // when we request real time data, we are given a handle so that we can cancel it upon closing

	// thread must be created after and destroyed before the callbacks
	std::atomic<bool> threadCancellationToken;
	std::thread readTickDataThread;

	bool _valid;
	bool _finished; //finished doesn't need to be atomic since the tickHandler thread runs on the same thread as readTickFile thread
};