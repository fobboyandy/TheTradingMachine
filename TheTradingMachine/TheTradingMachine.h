#pragma once

#include <queue>
#include <vector>
#include <memory>
#include <fstream>
#include "IBInterface.h"

using namespace std;

class TheTradingMachine
{

public:
	TheTradingMachine(string input);
	virtual ~TheTradingMachine();
	void requestTicks(function<void(const Tick& tick)> callback);

private:

	class IBInterfaceClient
	{
	public:
		IBInterfaceClient();
		~IBInterfaceClient();
		void requestRealTimeTicks(string ticker, function<void(const Tick&)> callback);

	private:
		IBInterface client;
		thread* messageProcess_;
		atomic<bool> threadRunning;
		void messageProcess(void);
		atomic<bool> clientReady;
		atomic<bool> clientValid;
	};

	class Position
	{
	public:
		string ticker;
		
	private:
		OrderId id;
	};


	fstream tickDataFile;
	bool realtime;
	string ticker;
	static IBInterfaceClient* ibapi;



protected:

};
