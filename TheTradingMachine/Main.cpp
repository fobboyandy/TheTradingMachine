/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#include "StdAfx.h"
#include <iostream>
#include <fstream>
#include <thread>
#include "IBInterface.h"
#include "TheTradingMachine.h"
#include <functional>

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

#define TICK_SAVE(TICKER) [](const Tick& tick) \
{ \
static string filename = string(TICKER) + string("TickData.csv");\
fstream output(filename, ios::app | ios::out);\
output << tick.tickType << ',';\
output << ctime(&tick.time) << ',';\
output << tick.price << ',';\
output << tick.size << ',';\
output << (int)tick.attributes.canAutoExecute << ',';\
output << (int)tick.attributes.pastLimit << ',';\
output << (int)tick.attributes.preOpen << ',';\
output << (int)tick.attributes.unreported << ',';\
output << (int)tick.attributes.bidPastLow << ',';\
output << (int)tick.attributes.askPastHigh << ',';\
output << tick.exchange << endl; \
output.close();\
}

class DataRecorder : public TheTradingMachine
{
public:
	DataRecorder(string ticker)
	{
		cout << "requesting ticks for " << ticker << endl;
		ibapi.requestRealTimeTicks(ticker, [this](const Tick& tick) {this->tickHandler(tick); });

		string filename = ticker + "TickData.csv";
		output.open(filename, ios::trunc | ios::out);
	}
	~DataRecorder()
	{
		output.close();
	}
	void tickHandler(const Tick& tick)
	{
		output << tick.tickType << ','; \
		output << ctime(&tick.time) << ','; \
		output << tick.price << ','; \
		output << tick.size << ','; \
		output << (int)tick.attributes.canAutoExecute << ','; \
		output << (int)tick.attributes.pastLimit << ','; \
		output << (int)tick.attributes.preOpen << ','; \
		output << (int)tick.attributes.unreported << ','; \
		output << (int)tick.attributes.bidPastLow << ','; \
		output << (int)tick.attributes.askPastHigh << ','; \
		output << tick.exchange << endl; \
	}

private:
	fstream output;
protected:

};

/* IMPORTANT: always use your paper trading account. The code below will submit orders as part of the demonstration. */
/* IB will not be responsible for accidental executions on your live account. */
/* Any stock or option symbols displayed are for illustrative purposes only and are not intended to portray a recommendation. */
/* Before contacting our API support team please refer to the available documentation. */
int main(int argc, char** argv)
{
	////
	//// start a message proessing thread
	////
	//thread IbMessageThread(IBClientMessaggeThread);
	//while (!clientReady.load())
	//{
	//	if (!clientValid.load())
	//	{
	//		cout << "Failed to startup client" << endl;
	//		system("pause");
	//	}
	//}

	//if(clientValid.load())
	//{
	//	client.requestRealTimeTicks("AMD", TICK_SAVE("AMD"));
	//	client.requestRealTimeTicks("VOO", TICK_SAVE("VOO"));
	//	client.requestRealTimeTicks("MU", TICK_SAVE("MU"));
	//	client.requestRealTimeTicks("NVDA", TICK_SAVE("NVDA"));

	//} 

	//while (clientValid.load())
	//{
	//	Sleep(10);
	//}


	//TheTradingMachine tm,tm2,tm3;


	DataRecorder amdTickRecorder("AMD");
	DataRecorder vooTickRecorder("VOO");
	DataRecorder muTickRecorder("MU");
	DataRecorder nvdaTickRecorder("NVDA");

	while (1)
	{
		Sleep(10);
	}
}
