#pragma once

#include <iostream>
#include <fstream>
#include "TheTradingMachine.h"

#define NUM_SECONDS_DAY 86400
#define RTH_SECONDS 48600
#define RTH_START 48600
#define RTH_END 72000

class TickRecorder : public TheTradingMachine
{
public:
	explicit TickRecorder(std::string input, std::shared_ptr<IBInterfaceClient> ibapi);
	~TickRecorder();
	void tickHandler(const Tick& tick);

private:
	std::fstream tickoutput;
	std::string ticker;
};
