#pragma once

#include <iostream>
#include <fstream>
#include "../../BaseAlgorithm/BaseAlgorithm/BaseAlgorithm.h"

#define NUM_SECONDS_DAY 86400
#define RTH_SECONDS 48600
#define RTH_START 48600
#define RTH_END 72000

class TickRecorder : public BaseAlgorithm
{
public:
	TickRecorder(std::string input, std::shared_ptr<InteractiveBrokersClient> ibInst, bool live);
	~TickRecorder();
	void tickHandler(const Tick& tick) override;
private:
	std::fstream tickoutput;
	std::string ticker;
};
