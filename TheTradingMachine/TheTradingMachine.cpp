#include "TheTradingMachine.h"
#include <iostream>
#include <functional>

using namespace std;

TheTradingMachine::IBInterfaceClient TheTradingMachine::ibapi;

TheTradingMachine::TheTradingMachine()
{
}

TheTradingMachine::~TheTradingMachine()
{
}

TheTradingMachine::IBInterfaceClient::IBInterfaceClient() :
	messageProcess_(nullptr)
{
	cout << "Initializing IB Client" << endl;
	clientReady.store(false);
	clientValid.store(true);
	threadRunning.store(false);
	if (!client.Initialize())
	{
	
		cout << "Unable to connect to neither TWS nor IB Gateway!" << endl;
		Sleep(10000);
		clientValid.store(false);
	}
	else
	{
		cout << "Successfully initialized IB Client." << endl;
		messageProcess_ = new thread(&TheTradingMachine::IBInterfaceClient::messageProcess, this);
		while (!clientReady.load())
		{
			Sleep(10);
		}
	}
}

TheTradingMachine::IBInterfaceClient::~IBInterfaceClient()
{
	//
	// Stop the thread
	//
	threadRunning.store(false);
	messageProcess_->join();
	delete messageProcess_;
	messageProcess_ = nullptr;
	client.disconnect();
}

void TheTradingMachine::IBInterfaceClient::requestRealTimeMinuteBars(string ticker, int timeFrameMinutes, function<void(const Bar&)> callback)
{
	client.requestRealTimeMinuteBars(ticker, timeFrameMinutes, callback);
}

void TheTradingMachine::IBInterfaceClient::requestHistoricalMinuteBars(string ticker, int timeFrameMinutes, function<void(const Bar&)> callback)
{
	client.requestHistoricalMinuteBars(ticker, timeFrameMinutes, callback);
}

void TheTradingMachine::IBInterfaceClient::requestRealTimeTicks(string ticker, function<void(const Tick&)> callback)
{
	client.requestRealTimeTicks(ticker, callback);
}

void TheTradingMachine::IBInterfaceClient::messageProcess(void)
{

	threadRunning.store(true);
	if (client.isConnected())
	{
		cout << "IB Message Processing Thread has started." << endl;
	}

	clientReady.store(true);
	while (client.isConnected() && threadRunning.load())
	{
		client.processMessages();
		Sleep(10);
	}

	clientValid.store(false);
}
