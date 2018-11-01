#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include "TickDataSource.h"

TickDataSource::TickDataSource(std::string input, std::shared_ptr<InteractiveBrokersClient> ibApiPtr) :
	input(input),
	ibApi(ibApiPtr),
	threadCancellationToken(false),
	dataStreamHandle(-1),
	_finished(false),
	_realTimeStream(false)
{
	if (input.find(".tickdat") != std::string::npos)
	{
		_realTimeStream = false;
	}
	else if (ibApiPtr != nullptr && ibApiPtr->isReady())
	{
		_realTimeStream = true;
	}
	else
	{
		throw std::runtime_error("Unable to initialize tick data source");
	}
}

TickDataSource::~TickDataSource()
{
	if (_realTimeStream && ibApi != nullptr)
	{
		ibApi->cancelRealTimeTicks(input, dataStreamHandle);	
	}
	else
	{
		// stop the thread if thread is joinable
		if (readTickDataThread.joinable())
		{
			//stop the thread
			threadCancellationToken = true;
			readTickDataThread.join();
		}
	}
}

bool TickDataSource::finished() const
{
	return _finished;
}

double TickDataSource::lastPrice() const
{
	return _lastPrice;
}

void TickDataSource::run() 
{	
	//
	// Check if it's a recorded data input for backtesting
	//
	if (!_realTimeStream)
	{
		//start a thread to read file
		readTickDataThread = std::thread([this]
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
		dataStreamHandle = ibApi->requestRealTimeTicks(input, [this](const Tick& tick)
		{
			this->broadcastTick(tick);
		});
		
	}
}

void TickDataSource::readTickFile(void)
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
	const std::vector<std::string>::size_type TICK_CSV_ROW_SZ = 12;
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
		broadcastTick(callbackTick);
	}

	//if threadcancellation was toggled, then it was forcefully terminated
	if (!threadCancellationToken)
		std::cout << "done reading from file" << std::endl;
	else
		std::cout << "terminated" << std::endl;

	_finished = true;
}

CallbackHandle TickDataSource::registerCallback(TickCallbackFunction callback)
{
	std::lock_guard<std::mutex> lock(callbackListMtx);
	callbackList[uniqueCallbackHandles] = callback;
	return uniqueCallbackHandles++;
}

void TickDataSource::unregisterCallback(CallbackHandle handle)
{
	std::lock_guard<std::mutex> lock(callbackListMtx);
	callbackList.erase(handle);
}

void TickDataSource::broadcastTick(const Tick & tick)
{
	//save the price before broadcasting the tick
	_lastPrice = tick.price;

	//dispatch the tick to the registered callbacks under a lock
	std::lock_guard<std::mutex> lock(callbackListMtx);
	for (auto& fn : callbackList)
	{
		// wrap the broadcast in a try catch in case
		// user didn't call stop and their
		// tickHandler goes out of scope before
		// BaseAlgorithm was able to unregister it.

		fn.second(tick);
	}
}
