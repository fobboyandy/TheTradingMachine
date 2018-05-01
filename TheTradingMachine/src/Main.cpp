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

#include "IBInterface.h"

const unsigned MAX_ATTEMPTS = 50;
const unsigned SLEEP_TIME = 10;

using namespace std;

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

	//initialize stock data structures 
	vector<vector<Stock>> watchList;


	for (;;) {
		++attempt;
		printf( "Attempt %u of %u\n", attempt, MAX_ATTEMPTS);

		IBInterface client;

		if( connectOptions) {
			client.setConnectOptions( connectOptions);
		}
		//! [connect]
		client.connect( host, port, clientId);
		
		//wait and process all the initial messages from TWS
		//TWS will be ready when TWS returns nextValidID
		while(!client.ready())
		{	
			Sleep(1000);
			client.processMessages();
		}
		//by this time the connection should be established and TWS is ready to accept commands
		//initialize the interface 
		client.initializeInterface();

		PythonSocketIPC stockDataPlot(DEFAULT_PORT);
		const Stock& amdStockData = client.requestStockCandles("AMD", "ISLAND");
		Candlebar lastCandle;
		while(client.isConnected())
		{
			client.processMessages();
			lastCandle = amdStockData.getLastCandle();
			//transmit it to python
			if (lastCandle.valid)
			{
				string valStr = to_string(lastCandle.close);
				stockDataPlot.send(valStr.c_str(), valStr.length());

			}
		}

		//! [ereader]
		if( attempt >= MAX_ATTEMPTS) {
			break;
		}

		printf( "Sleeping %u seconds before next attempt\n", SLEEP_TIME);
		sleep( SLEEP_TIME);
	}

	printf ( "End of C++ Socket Client Test\n");
}




//int main()
//{
//	PythonSocketIPC pythonIPC(DEFAULT_PORT);
//
//	double val = 1.234;
//	for (size_t i = 0; i < 10; i++)
//	{
//		string valStr = to_string((double)(i));
//		pythonIPC.send(valStr.c_str(), valStr.length() * sizeof(char));
//		Sleep(500);
//	}
//	return 0;
//}