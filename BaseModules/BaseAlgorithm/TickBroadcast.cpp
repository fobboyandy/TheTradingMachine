#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include "TickBroadcast.h"

TickBroadcast::TickBroadcast(std::string input, std::shared_ptr<InteractiveBrokersClient> ibApiPtr) :
	input_(input),
	ibApi_(ibApiPtr),
	threadCancellationToken_(false),
	dataStreamHandle_(-1),
	finished_(false),
	realTimeStream_(false)
{
	if (input.find(".tickdat") != std::string::npos)
	{
		realTimeStream_ = false;
	}
	else if (ibApiPtr != nullptr && ibApiPtr->isReady())
	{
		realTimeStream_ = true;
	}
	else
	{
		throw std::runtime_error("Unable to initialize tick data source");
	}
}

TickBroadcast::~TickBroadcast()
{
	if (realTimeStream_ && ibApi_ != nullptr)
	{
		ibApi_->cancelRealTimeTicks(input_, dataStreamHandle_);	
	}
	else
	{
		// stop the thread if thread is joinable
		if (readTickDataThread_.joinable())
		{
			//stop the thread
			threadCancellationToken_ = true;
			readTickDataThread_.join();
		}
	}
}

bool TickBroadcast::finished() const
{
	return finished_;
}

Tick TickBroadcast::lastTick() const
{
	std::lock_guard<std::mutex> tickLock(tickMtx_);
	return lastTick_;
}

void TickBroadcast::run()
{	
	//
	// Check if it's a recorded data input for backtesting
	//
	if (!realTimeStream_)
	{
		//start a thread to read file
		readTickDataThread_ = std::thread([this]
		{
			// new thread that reads tick data from a file and calls derived
			// classes' tickhandler (calls preTickHandler first)
			readTickFile();
		});
	}
	else 
	{
		// register for a live data stream from ib and have ticks sent to 
		// preTickDispatch handler function. preTickDispatch will validate
		// callback function's existence before calling to prevent calling
		// invalidated lambda functions.
		dataStreamHandle_ = ibApi_->requestRealTimeTicks(input_, [this](const Tick& tick)
		{
			this->broadcastTick(tick);
		});
		
	}
}

void TickBroadcast::readTickFile(void)
{
	std::fstream tickDataFile(input_, std::ios::in);
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
	const std::vector<std::string>::size_type TICK_CSV_ROW_SZ = 12;
	std::string currLine;
	std::vector<std::string> csvRow(TICK_CSV_ROW_SZ);
	//
	// For each row in the csv, parse out the values in string
	// and reconstruct the tick data.
	//
	while (std::getline(tickDataFile, currLine) && !threadCancellationToken_)
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
		broadcastTick(callbackTick);
	}

	//if threadcancellation was toggled, then it was forcefully terminated
	if (!threadCancellationToken_)
		std::cout << "done reading from file" << std::endl;
	else
		std::cout << "terminated" << std::endl;

	finished_ = true;
}

CallbackHandle TickBroadcast::registerListener(TickListener callback)
{
	std::lock_guard<std::mutex> lock(callbackListMtx_);
	listeners_[uniqueCallbackHandles_] = callback;
	return uniqueCallbackHandles_++;
}

void TickBroadcast::unregisterCallback(CallbackHandle handle)
{
	std::lock_guard<std::mutex> lock(callbackListMtx_);
	listeners_.erase(handle);
}

void TickBroadcast::broadcastTick(const Tick & tick)
{
	// don't broadcast if unreported tick
	if(!tick.attributes.unreported)
	{
		std::unique_lock<std::mutex> tickLock(tickMtx_);
		//save the tick before broadcasting
		lastTick_ = tick;
		tickLock.unlock();

		//dispatch the tick to the registered callbacks under a lock
		std::lock_guard<std::mutex> lock(callbackListMtx_);
		for (auto& fn : listeners_)
		{
			// wrap the broadcast in a try catch in case
			// user didn't call stop and their
			// tickHandler goes out of scope before
			// BaseAlgorithm was able to unregister it.

			fn.second(lastTick_);
		}
	}
}
