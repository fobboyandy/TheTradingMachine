/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#include "../IBInterface/StdAfx.h"

#include <iostream>
#include "CandleMaker.h"
#include "../IBInterface/IBInterface.h"
#include "TheTradingMachine.h"
#include "SupportBreakShort.h"
#include "TickRecorder.h"

/* IMPORTANT: always use your paper trading account. The code below will submit orders as part of the demonstration. */
/* IB will not be responsible for accidental executions on your live account. */
/* Any stock or option symbols displayed are for illustrative purposes only and are not intended to portray a recommendation. */
/* Before contacting our API support team please refer to the available documentation. */
int main(int argc, char** argv)
{

	//TickRecorder record1("AMD");
	//TickRecorder record2("NVDA");
	//TickRecorder record3("AMZN");
	
	SupportBreakShort test("D:\\Users\\fobboyandy\\Desktop\\TheTradingMachine\\x64\\Debug\\Jul 19AMD.tickdat");

	std::cout << "done" << std::endl;
	while (1)
	{
		Sleep(10);
	}
}