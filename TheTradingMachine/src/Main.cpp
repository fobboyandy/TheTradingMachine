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

#include "TestCppClient.h"

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
	printf( "Start of C++ Socket Client Test %u\n", attempt);

	for (;;) {
		++attempt;
		printf( "Attempt %u of %u\n", attempt, MAX_ATTEMPTS);

		TestCppClient client;

		if( connectOptions) {
			client.setConnectOptions( connectOptions);
		}
		//! [connect]
		client.connect( host, port, clientId);

		//does another thread even update the isConnected status?
		int i = 0;
		while(!client.isConnected() && i < 5) 
		{
			std :: cout << "Failed to connect. Attempt " << i++ << endl;
			sleep(SLEEP_TIME);
		}

		client.processMessages();

		//by this time the connection should be established

		client.testfn();
		
		client.processMessages();



		//! [ereader]
		if( attempt >= MAX_ATTEMPTS) {
			break;
		}

		printf( "Sleeping %u seconds before next attempt\n", SLEEP_TIME);
		sleep( SLEEP_TIME);
	}

	printf ( "End of C++ Socket Client Test\n");
}

