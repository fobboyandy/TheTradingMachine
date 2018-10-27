#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include "Common.h"
#include "../IBInterfaceClient/IBInterfaceClient.h"

class TickDataSource
{
public:
	TickDataSource(std::string in);
	TickDataSource(std::string in, std::weak_ptr<IBInterfaceClient> ibApiPtr);
	~TickDataSource();
	CallbackHandle registerCallback(TickCallbackFunction callback);
	void unregisterCallback(CallbackHandle handle);
	bool valid() const;
	bool finished() const;
	double lastPrice() const;

	// we want the data source to start running when the parent has set up everything
	// if we don't have a run function, the ticks would be fired before the parent 
	// has a chance to construct everything else. this would cause loss of data in the case
	// of playback mode
	void run();

private:
	void preTickDispatch(const Tick& tick);
	void readTickFile(void);
	
private:	
	// price is updated from another thread. make atomic
	std::atomic<double> _lastPrice;

	std::mutex callbackListMtx;
	CallbackHandle uniqueCallbackHandles;
	std::unordered_map<CallbackHandle, TickCallbackFunction> callbackList;

	std::string input;
	const bool _realTimeStream;

	// api data
	std::weak_ptr<IBInterfaceClient> ibApi; 
	// when we request real time data, we are given a handle so that we can cancel it upon closing
	CallbackHandle dataStreamHandle;

	// thread must be created after and destroyed before the callbacks
	std::atomic<bool> threadCancellationToken;
	std::thread readTickDataThread;

	bool _valid;
	bool _finished; //finished doesn't need to be atomic since the tickHandler thread runs on the same thread as readTickFile thread
};