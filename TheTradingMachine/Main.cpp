/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */


//#include "StdAfx.h"

#include <iostream>
#include "StdAfx.h"
#include "IBInterface.h"

/* IMPORTANT: always use your paper trading account. The code below will submit orders as part of the demonstration. */
/* IB will not be responsible for accidental executions on your live account. */
/* Any stock or option symbols displayed are for illustrative purposes only and are not intended to portray a recommendation. */
/* Before contacting our API support team please refer to the available documentation. */
int main(int argc, char** argv)
{
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
	
		//client.historicalData();

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


