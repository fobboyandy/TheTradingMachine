#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include "Common.h"

class TickBroadcast
{
public:
	TickBroadcast(std::string input, std::shared_ptr<InteractiveBrokersClient> ibApiPtr);
	~TickBroadcast();

	CallbackHandle registerListener(TickListener callback);
	void unregisterCallback(CallbackHandle handle);
	bool finished() const;
	Tick lastTick() const;

	// we want the data source to start running when the parent has set up everything
	// if we don't have a run function, the ticks would be fired before the parent 
	// has a chance to construct everything else. this would cause loss of data in the case
	// of playback mode
	void run();

private:
	void broadcastTick(const Tick& tick);
	void readTickFile(void);
	
private:	
	// tick is written from another thread. protect with lock 
	mutable std::mutex tickMtx_;
	Tick lastTick_;

	std::mutex callbackListMtx_;
	CallbackHandle uniqueCallbackHandles_;
	std::unordered_map<CallbackHandle, TickListener> listeners_;

	std::string input_;
	bool realTimeStream_;

	// api data
	std::shared_ptr<InteractiveBrokersClient> ibApi_;
	// when we request real time data, we are given a handle so that we can cancel it upon closing
	CallbackHandle dataStreamHandle_;

	// thread must be created after and destroyed before the callbacks
	std::atomic<bool> threadCancellationToken_;
	std::thread readTickDataThread_;

	bool valid_;
	bool finished_; //finished doesn't need to be atomic since the tickHandler thread runs on the same thread as readTickFile thread
};