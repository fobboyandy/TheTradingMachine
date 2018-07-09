/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */


//#include "StdAfx.h"

#include <iostream>
#include <fstream>
#include "StdAfx.h"
#include "IBInterface.h"
#include <thread>

#define BAR_SAVE(TICKER, TIMEFRAME) [](const Bar& bar) \
{ \
static string filename = TICKER + to_string(TIMEFRAME) + "min" + bar.time.substr(0, 8) + ".csv";\
fstream output(filename, ios::app | ios::out);\
output << bar.time << ',';\
output << bar.open << ',';\
output << bar.high << ',';\
output << bar.low << ',';\
output << bar.close << ',';\
output << bar.volume << ',';\
output << bar.wap << endl;\
output.close();\
}

IBInterface client;
atomic<bool> clientReady(false);
atomic<bool> clientValid(true);

void IBClientMessaggeThread(void)
{
	if (!client.Initialize())
	{
		cout << "Unable to connect to neither TWS nor IB Gateway!" << endl;
		Sleep(10000);
		clientValid.store(false);
		return;
	}
	clientReady.store(true);
	
	// Process messages in this thread
	while (client.isConnected())
	{
		client.processMessages();
		Sleep(10);
	}

	//
	// If somehow it got disconnected, set valid to false
	//
	clientValid.store(false);
}



/* IMPORTANT: always use your paper trading account. The code below will submit orders as part of the demonstration. */
/* IB will not be responsible for accidental executions on your live account. */
/* Any stock or option symbols displayed are for illustrative purposes only and are not intended to portray a recommendation. */
/* Before contacting our API support team please refer to the available documentation. */
int main(int argc, char** argv)
{
	//
	// start a message proessing thread
	//
	thread IbMessageThread(IBClientMessaggeThread);
	while (!clientReady.load())
	{
		if (!clientValid.load())
		{
			cout << "Failed to startup client" << endl;
			system("pause");
		}
	}

	if(clientValid.load())
	{
		client.requestHistoricalMinuteBars("AMD", 1, BAR_SAVE("MU", 1));
		client.requestHistoricalMinuteBars("MU", 1, BAR_SAVE("MU", 1));
		client.requestHistoricalMinuteBars("EXPR", 1, BAR_SAVE("NVDA", 1));
		client.requestHistoricalMinuteBars("VOO", 1, BAR_SAVE("VOO", 1));
	} 

	while (clientValid.load())
	{
		Sleep(10);
	}

}
