#include "TickRecorder.h"
#include <windows.h>
#include <time.h>

std::string TimeToString(time_t time)
{
	char timeStr[256];
	ctime_s(timeStr, 256, &time);
	return std::string(timeStr);
}

TickRecorder::TickRecorder(std::string input, std::shared_ptr<IBInterfaceClient> ibInst, bool live):
	BaseAlgorithm(input, ibInst, live)
{
	run();
}

TickRecorder::~TickRecorder()
{
	stop();
	tickoutput.close();
}

void TickRecorder::tickHandler(const Tick & tick)
{
	//should store binary data instead of strings
	auto tickTime = tick.time;
	std::string s = TimeToString(time(&tickTime));
	tickoutput << tick.tickType << ',';
	tickoutput << tick.time << ",";
	tickoutput << s.substr(0, s.length() - 1) << ",";
	tickoutput << tick.price << ',';
	tickoutput << tick.size << ',';
	tickoutput << (int)tick.attributes.canAutoExecute << ',';
	tickoutput << (int)tick.attributes.pastLimit << ',';
	tickoutput << (int)tick.attributes.preOpen << ',';
	tickoutput << (int)tick.attributes.unreported << ',';
	tickoutput << (int)tick.attributes.bidPastLow << ',';
	tickoutput << (int)tick.attributes.askPastHigh << ',';
	tickoutput << tick.exchange << std::endl;
}

EXPORT_ALGORITHM(TickRecorder)