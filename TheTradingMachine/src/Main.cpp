/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#include "StdAfx.h"

#ifdef _WIN32
# include <Windows.h>
# define sleep( seconds) Sleep( seconds * 1000);
#else
# include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include "Broker.h"
#include "TheTradingMachine.h"
#include "PythonSocketIPC.h"
#include "Python.h"

#include "IBInterface.h"

const unsigned SLEEP_TIME = 10;

using namespace std;


//Transmits a candle every ~5 seconds
void PythonCandleIpcThread(const Stock* stock)
{
	//start the server script on python

	//initialize the socket ipc 
	PythonSocketIPC pythonIpc(DEFAULT_PORT); 
	Candlebar c = stock->getLastCandle();
	
	//block until we get the first valid candlebar
	while (!c.valid)
	{
		c = stock->getLastCandle();
		Sleep(500);
	}
	while (1)
	{
		 c = stock->getLastCandle();
		 pythonIpc.send(c.open);
		 pythonIpc.send(c.high);
		 pythonIpc.send(c.low);
		 pythonIpc.send(c.close);
		 //This will more or less be 5 seconds. Small drifts may accumulate but should be negligible in the long run?
		 Sleep(5000);
	}

}


/* IMPORTANT: always use your paper trading account. The code below will submit orders as part of the demonstration. */
/* IB will not be responsible for accidental executions on your live account. */
/* Any stock or option symbols displayed are for illustrative purposes only and are not intended to portray a recommendation. */
/* Before contacting our API support team please refer to the available documentation. */
int main(int argc, char** argv)
{
	const char* host = argc > 1 ? argv[1] : "";
	unsigned int port = argc > 2 ? atoi(argv[2]) : 0;
	if (port <= 0)
		port = 7496;
	const char* connectOptions = argc > 3 ? argv[3] : "";
	int clientId = 0;

	unsigned attempt = 0;
	bool threadStarted = false;

	while(1)
	{
		IBInterface client;
		//if attempt to reconnect fails after certain amount of attempts, program exits
		if (!client.Initialize())
		{
			cout << "Unable to connect to neither TWS nor IB Gateway!" << endl;
			Sleep(10000);
			return -1;
		}

		const Stock& amdStockData = client.requestStockCandles("AMD", "ISLAND");

		thread pythonIpcThread(PythonCandleIpcThread, &amdStockData);

		//main thread processes messages
		while (client.isConnected())
		{
			client.processMessages();
		}

		//if the program was able to connect previously and suddenly fails, we restart the loop and try to reconnect
		//if the reconnection fails, the program exits
		cout << "Client got disconnected from either TWS or IB Gateway. Reconnecting..." << endl;

	}
	return 0;
}

