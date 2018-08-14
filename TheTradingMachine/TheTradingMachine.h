#pragma once

#include <queue>
#include <vector>
#include <memory>
#include <fstream>
#include "IBInterface.h"

class TheTradingMachine
{

public:
	TheTradingMachine(std::string input);
	virtual ~TheTradingMachine();
	void requestTicks(std::function<void(const Tick& tick)> callback);

private:

	class IBInterfaceClient
	{
	public:
		IBInterfaceClient();
		~IBInterfaceClient();
		void requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback);

	private:
		IBInterface client;
		std::thread* messageProcess_;
		std::atomic<bool> threadRunning;
		void messageProcess(void);
		std::atomic<bool> clientReady;
		std::atomic<bool> clientValid;
	};

	std::fstream tickDataFile;
	bool realtime;
	std::string ticker;
	static IBInterfaceClient* ibapi;



protected:
};
