#pragma once

#include <queue>
#include <vector>
#include <memory>
#include "IBInterface.h"

using namespace std;

class TheTradingMachine
{

public:
	TheTradingMachine();
	~TheTradingMachine();

private:

	class IBInterfaceClient
	{
	public:
		IBInterfaceClient();
		~IBInterfaceClient();

		void requestRealTimeMinuteBars(string ticker, int timeFrameMinutes, function<void(const Bar&)> callback);
		void requestHistoricalMinuteBars(string ticker, int timeFrameMinutes, function<void(const Bar&)> callback);
		void requestRealTimeTicks(string ticker, function<void(const Tick&)> callback);

	private:
		IBInterface client;
		thread* messageProcess_;
		atomic<bool> threadRunning;
		void messageProcess(void);
		atomic<bool> clientReady;
		atomic<bool> clientValid;
	};


protected:

	static IBInterfaceClient ibapi;


};