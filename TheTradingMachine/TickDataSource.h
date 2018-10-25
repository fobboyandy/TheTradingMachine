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
private:
	std::string input;
	std::shared_ptr<IBInterfaceClient> ibApi;
	std::thread readTickDataThread;
	int dataStreamHandle;
	std::atomic<bool> threadCancellationToken;
	int streamingDataHandle; // when we request real time data, we are given a handle so that we can cancel it upon closing
	void readTickFile(void);
	bool _valid;
	bool _finished; //finished doesn't need to be atomic since the tickHandler thread runs on the same thread as readTickFile thread

	std::function<void(const Tick& tick)> tickDataDispatchCallback;
};