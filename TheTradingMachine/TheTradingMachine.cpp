#include <iostream>
#include <functional>
#include <sstream>
#include <thread>
#include <assert.h>
#include "TheTradingMachine.h"

#define TICK_CSV_ROW_SZ 12

TheTradingMachine::TheTradingMachine(std::string in, IBInterfaceClient* ibApiPtr) :
	realtime(false),
	input(new std::string(in)),
	tickDataFile(nullptr),
	ibapi(ibApiPtr),
	readTickDataThread(nullptr),
	runReadTickDataThread(new std::atomic<bool>(false)),
	plotData(new std::shared_ptr<PlotData>(new PlotData))
{
	
}

TheTradingMachine::~TheTradingMachine()
{
	// stop should have been called so simply delete variables at this point
	if (readTickDataThread != nullptr)
	{
		delete readTickDataThread;
		readTickDataThread = nullptr;
	}

	// independently delete this pointer in case the above case failed
	if (runReadTickDataThread != nullptr)
	{
		delete runReadTickDataThread;
		runReadTickDataThread = nullptr;
	}

	if (tickDataFile != nullptr)
	{
		tickDataFile->close();
		delete tickDataFile;
		tickDataFile = nullptr;
	}

	if (input != nullptr)
	{
		delete input;
		input = nullptr;
	}

	// this is a pointer to a shared pointer. only if the last owner
	// deletes the shared pointer will it be destructed.
	if (plotData != nullptr)
	{
		delete plotData;
		plotData = nullptr;
	}
}

// this function is used to handle any base class tick handling before passing it
// to derived algorithm for specific tick handling. eg. every algorithm must save
// plot data before processing so it is handled here
void TheTradingMachine::preTickHandler(const Tick & tick)
{
	if (*plotData != nullptr)
	{
		PlotData& plotDataRef = **plotData;
		plotDataRef.ticks.push_back(tick);
	}
	//algorithm specific callback
	tickHandler(tick);
}

void TheTradingMachine::readTickFile(void)
{
	enum CsvIndex {
		tickTypeIndex = 0,
		timeIndex,
		priceIndex = 3,
		sizeIndex,
		canAutoExecuteIndex,
		pastLimitIndex,
		preOpenIndex,
		unreportedIndex,
		bidPastLowIndex,
		askPastHIndexigh,
		exchangeIndex
	};
	std::string currLine;
	std::vector<std::string> csvRow(TICK_CSV_ROW_SZ);
	//
	// For each row in the csv, parse out the values in string
	// and reconstruct the tick data.
	//
	while (std::getline(*tickDataFile, currLine) && runReadTickDataThread->load())
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
			case tickTypeIndex:
				callbackTick.tickType = std::stoi(token);
				break;
			case timeIndex:
				callbackTick.time = static_cast<time_t>(stoll(token));
				break;
			case priceIndex:
				callbackTick.price = std::stod(token);
				break;
			case sizeIndex:
				callbackTick.size = std::stoi(token);
				break;
			case canAutoExecuteIndex:
				callbackTick.attributes.canAutoExecute = static_cast<bool>(std::stoi(token));
				break;
			case pastLimitIndex:
				callbackTick.attributes.pastLimit = static_cast<bool>(std::stoi(token));
				break;
			case preOpenIndex:
				callbackTick.attributes.preOpen = static_cast<bool>(std::stoi(token));
				break;
			case unreportedIndex:
				callbackTick.attributes.unreported = static_cast<bool>(std::stoi(token));
				break;
			case bidPastLowIndex:
				callbackTick.attributes.bidPastLow = static_cast<bool>(std::stoi(token));
				break;
			case askPastHIndexigh:
				callbackTick.attributes.askPastHigh = static_cast<bool>(std::stoi(token));
				break;
			case exchangeIndex:
				callbackTick.exchange = token;
				break;
			}
		}
		preTickHandler(callbackTick);
	}
	if (runReadTickDataThread->load())
		std::cout << "done reading from file" << std::endl;
	else
		std::cout << "terminated" << std::endl;
}

void TheTradingMachine::start(void)
{
	//
	// Check if it's a recorded data input for backtesting
	//
	if (input->find(".tickdat") != std::string::npos)
	{
		tickDataFile = new std::fstream(*input, std::ios::in);
		std::cout << "from file" << std::endl;
		runReadTickDataThread->store(true);
		readTickDataThread = new std::thread([this] 
		{
			// new thread that reads tick data from a file and calls derived
			// classes' tickhandler (calls preTickHandler first)
			readTickFile();
		});
	}
	else if (ibapi != nullptr && ibapi->isReady())
	{
		//
		// If input wasn't a tick historical file, and an ibapi was provided, then we can assume it's a ticker
		// for real time trading.
		//
		realtime = true;
		ibapi->requestRealTimeTicks(*input, [this](const Tick& tick) {preTickHandler(tick); });
	}
	else
	{
		throw std::runtime_error("Must provide a valid Interactive Broker Connection or historical data file!");
	}
}

void TheTradingMachine::stop(void)
{
	// signal the file reading thread to stop running
	if (readTickDataThread != nullptr)
	{
		assert(runReadTickDataThread != nullptr);
		runReadTickDataThread->store(false);
		// we have to join the function instead of detaching because the thread
		// runs on a derived member class. therefore, we must guarantee to the
		// caller that the thread has stopped and that the derived class can be 
		// destroyed after this function returns
		if (readTickDataThread->joinable())
			readTickDataThread->join();
	}
	else if (ibapi != nullptr && ibapi->isReady())
	{
		//make sure all positions are closed and unregister tick handlers
	}
	else
	{
		assert(false);
	}

}

IBInterfaceClient::IBInterfaceClient() :
	messageProcess_(nullptr),
	client(new IBInterface),
	runThread(new std::atomic<bool>)
{
	std::cout << "Initializing IB Client" << std::endl;
	runThread->store(false);

	if (client != nullptr)
	{
		//connect to ib api
		if(client->Initialize())
		{
			messageProcess_ = new std::thread(&IBInterfaceClient::messageProcess, this);
			//
			// Wait for message handling thread to initialize
			//
			while (!runThread->load())
			{
				Sleep(10);
			}
			std::cout << "Successfully initialized IB Client." << std::endl;
			
		}
		else
		{
			std::cout << "Unable to connect to neither TWS nor IB Gateway!" << std::endl;
			delete client;
			client = nullptr;
			Sleep(10000);
		}
	}

}

IBInterfaceClient::~IBInterfaceClient()
{
	if (runThread != nullptr && runThread->load())
	{
		//
		// Stop the thread
		//
		runThread->store(false);
		if (messageProcess_ != nullptr)
		{
			messageProcess_->join();
			delete messageProcess_;
			messageProcess_ = nullptr;
		}
		delete runThread;
		runThread = nullptr;
	}

	if (client != nullptr)
	{
		client->disconnect();
		delete client;
		client = nullptr;
	}
}

void IBInterfaceClient::requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback)
{
	if (client != nullptr)
	{
		client->requestRealTimeTicks(ticker, callback);
	}
}

bool IBInterfaceClient::isReady(void)
{
	// runThread is true when the connection has been initialized and the 
	// messaging thread has started
	return runThread->load();
}

void IBInterfaceClient::messageProcess(void)
{
	if (client->isConnected())
	{
		std::cout << "IB Message Processing Thread has started." << std::endl;
		runThread->store(true);
	}
	//shouldnt need to check for nulls here because we can only get here if these are not nullptr
	while (client->isConnected() && runThread->load())
	{
		client->processMessages();
		Sleep(10);
	}
}

THETRADINGMACHINEDLL IBInterfaceClient* InitializeIbInterfaceClient(void)
{
	static IBInterfaceClient* ibInterfaceClient = nullptr;
	if (ibInterfaceClient == nullptr)
	{
		ibInterfaceClient = new IBInterfaceClient();
	}

	return ibInterfaceClient;
}
