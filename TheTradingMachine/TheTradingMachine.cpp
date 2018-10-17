#include <iostream>
#include <functional>
#include <atomic>
#include <sstream>
#include <fstream>
#include <thread>
#include <assert.h>
#include "TheTradingMachine.h"

#define TICK_CSV_ROW_SZ 12

class TheTradingMachine::TheTradingMachineImpl
{
public:
	explicit TheTradingMachineImpl(TheTradingMachine* parent, std::string in, std::shared_ptr<IBInterfaceClient> ibApiPtr);
	~TheTradingMachineImpl();
	std::shared_ptr<PlotData> plotData;

	void start(void);
	void stop(void);
	bool valid(void) const;

private:
	enum class Mode
	{
		REALTIME,
		PLAYBACK
	};

	TheTradingMachine* parent;

	void preTickHandler(const Tick& tick);
	Mode operationMode;
	std::string input;
	int dataStreamHandle;
	std::shared_ptr<IBInterfaceClient> ibApi;
	std::thread readTickDataThread;
	std::atomic<bool> threadCancellationToken;
	int streamingDataHandle; // when we request real time data, we are given a handle so that we can cancel it upon closing
	bool valid_;
	void readTickFile(void);
};

TheTradingMachine::TheTradingMachineImpl::TheTradingMachineImpl(TheTradingMachine* parentIn, std::string in, std::shared_ptr<IBInterfaceClient> ibApiPtr):
	parent(parentIn),
	input(in),
	ibApi(ibApiPtr),
	plotData(std::make_shared<PlotData>()),
	threadCancellationToken(false),
	valid_(false),
	dataStreamHandle(0)
{
	//
	// Check if it's a recorded data input for backtesting
	//
	if (input.find(".tickdat") != std::string::npos)
	{
		operationMode = Mode::PLAYBACK;
		valid_ = true;
	}
	else if (ibApi != nullptr && ibApi->isReady())
	{
		operationMode = Mode::REALTIME;
		valid_ = true;
	}
}

TheTradingMachine::TheTradingMachineImpl::~TheTradingMachineImpl()
{
	stop();
}

void TheTradingMachine::TheTradingMachineImpl::start(void)
{
	if (!valid_)
	{
		// do nothing if not valid
		return;
	}
	switch (operationMode)
	{
		case Mode::REALTIME:
			dataStreamHandle = ibApi->requestRealTimeTicks(input, [this](const Tick& tick) {preTickHandler(tick); });
			break;

		case Mode::PLAYBACK:
			//start a thread to read file
			readTickDataThread = std::thread([this]
			{
				// new thread that reads tick data from a file and calls derived
				// classes' tickhandler (calls preTickHandler first)
				readTickFile();
			});
			break;

		default:
			break;
	}
}

void TheTradingMachine::TheTradingMachineImpl::stop(void)
{
	if (!valid_)
	{
		return;
	}
	switch (operationMode)
	{
		case Mode::REALTIME:
			ibApi->cancelRealTimeTicks(input, dataStreamHandle);
			break;

		case Mode::PLAYBACK:
			//stop the thread
			threadCancellationToken = true;
			if (readTickDataThread.joinable())
			{
				readTickDataThread.join();
			}
			break;

		default:
			break;
	}

}

bool TheTradingMachine::TheTradingMachineImpl::valid(void) const
{
	return valid_;
}

void TheTradingMachine::TheTradingMachineImpl::preTickHandler(const Tick & tick)
{
	// first handle the tick algorithm specific callback
	parent->tickHandler(tick);

	// in case if gui is sometimes slow and is holding the lock, we don't want to block this thread
	// therefore, if we fail to retrieve the lock immediately, we push the data into a secondary buffer.
	// when we successfully get the lock, we simply have to move the data into plotData. this will allow
	// the algorithm to be more overall responsive .
	std::unique_lock<std::mutex> lock(plotData->plotDataMtx, std::defer_lock);
	PlotData& plotDataRef = *plotData;
	//this is the only thread that accesses buffer no synchronization needed
	plotDataRef.buffer.push(tick);
	// if it was able to lock the lock, then unload everything from the buffer into the plotData
	// the buffer should usually 
	if (lock.try_lock())
	{
		while (!plotDataRef.buffer.empty())
		{
			plotDataRef.ticks.push_back(plotDataRef.buffer.front());
			plotDataRef.buffer.pop();
		}
		lock.unlock();
	}
}

void TheTradingMachine::TheTradingMachineImpl::readTickFile(void)
{
	std::fstream tickDataFile(input, std::ios::in);
	enum class CsvIndex {
		TICKTYPEINDEX = 0,
		TIMEINDEX,
		PRICEINDEX = 3,
		SIZEINDEX,
		CANAUTOEXECUTEINDEX,
		PASTLIMITINDEX,
		PREOPENINDEX,
		UNREPORTEDINDEX,
		BIDPASTLOWINDEX,
		ASKPASTHINDEXIGH,
		EXCHANGEINDEX
	};
	std::string currLine;
	std::vector<std::string> csvRow(TICK_CSV_ROW_SZ);
	//
	// For each row in the csv, parse out the values in string
	// and reconstruct the tick data.
	//
	while (std::getline(tickDataFile, currLine) && !threadCancellationToken)
	{
		std::stringstream s(currLine);
		std::string token;
		Tick callbackTick;
		//
		// Get each token separated by , and reconstruct the tick
		// with each csv row
		//
		for (size_t i = 0; std::getline(s, token, ','); i++)
		{
			switch (static_cast<CsvIndex>(i))
			{
			case CsvIndex::TICKTYPEINDEX:
				callbackTick.tickType = std::stoi(token);
				break;
			case CsvIndex::TIMEINDEX:
				callbackTick.time = static_cast<time_t>(stoll(token));
				break;
			case CsvIndex::PRICEINDEX:
				callbackTick.price = std::stod(token);
				break;
			case CsvIndex::SIZEINDEX:
				callbackTick.size = std::stoi(token);
				break;
			case CsvIndex::CANAUTOEXECUTEINDEX:
				callbackTick.attributes.canAutoExecute = static_cast<bool>(std::stoi(token));
				break;
			case CsvIndex::PASTLIMITINDEX:
				callbackTick.attributes.pastLimit = static_cast<bool>(std::stoi(token));
				break;
			case CsvIndex::PREOPENINDEX:
				callbackTick.attributes.preOpen = static_cast<bool>(std::stoi(token));
				break;
			case CsvIndex::UNREPORTEDINDEX:
				callbackTick.attributes.unreported = static_cast<bool>(std::stoi(token));
				break;
			case CsvIndex::BIDPASTLOWINDEX:
				callbackTick.attributes.bidPastLow = static_cast<bool>(std::stoi(token));
				break;
			case CsvIndex::ASKPASTHINDEXIGH:
				callbackTick.attributes.askPastHigh = static_cast<bool>(std::stoi(token));
				break;
			case CsvIndex::EXCHANGEINDEX:
				callbackTick.exchange = token;
				break;
			}
		}
		preTickHandler(callbackTick); 
	}
	
	//if threadcancellation was toggled, then it was forcefully terminated
	if (!threadCancellationToken)
		std::cout << "done reading from file" << std::endl;
	else
		std::cout << "terminated" << std::endl;

	// we need to indicate that this is the end of the algorithm. we need to block here to unload
	// any remaining plot data left in the buffer
	std::lock_guard<std::mutex> lock(plotData->plotDataMtx);

	while (!plotData->buffer.empty())
	{
		plotData->ticks.push_back(plotData->buffer.front());
		plotData->buffer.pop();
	}

	plotData->finished = true;
}

TheTradingMachine::TheTradingMachine(std::string in, std::shared_ptr<IBInterfaceClient> ibApiPtr) :
	impl_(new TheTradingMachineImpl(this, in, ibApiPtr))
{
}

TheTradingMachine::~TheTradingMachine()
{
	if (impl_ != nullptr)
	{
		delete impl_;
		impl_ = nullptr;
	}
}

void TheTradingMachine::start(void)
{
	impl_->start();
}

void TheTradingMachine::stop(void)
{
	impl_->stop();
}

std::shared_ptr<PlotData> TheTradingMachine::getPlotPlotData()
{
	return impl_->plotData;
}

bool TheTradingMachine::valid() const
{
	if (impl_ == nullptr)
		return false;
	return impl_->valid();
}
