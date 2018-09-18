#include <iostream>
#include <functional>
#include <sstream>
#include <thread>
#include "TheTradingMachine.h"


#define TICK_CSV_ROW_SZ 12

TheTradingMachine::TheTradingMachine(std::string input, IBInterfaceClient* ibApiPtr) :
	realtime(false), 
	ibapi(ibApiPtr),
	readTickDataThread(nullptr)
{
	//
	// Check if it's a recorded data input for backtesting
	//
	if (input.find(".tickdat") != std::string::npos)
	{
		tickDataFile = new std::fstream(input, std::ios::in);
	}
	else if (ibapi != nullptr)
	{
		//
		// If input wasn't a tick historical file, and an ibapi was provided, then we can assume it's a ticker
		// for real time trading.
		//
		ticker = new std::string(input);
		realtime = true;
	}
	else
	{
		throw std::runtime_error("Must provide an IB Interface or historical data!");
	}
}

TheTradingMachine::~TheTradingMachine()
{
	if (tickDataFile != nullptr)
	{
		tickDataFile->close();
		delete tickDataFile;
		tickDataFile = nullptr;
	}
	if (ticker != nullptr)
	{
		delete ticker;
		ticker = nullptr;
	}
	if (readTickDataThread != nullptr)
	{
		readTickDataThread->join();
		delete readTickDataThread;
		readTickDataThread = nullptr;
	}
}

void TheTradingMachine::requestTicks(std::function<void(const Tick& tick)> callback) 
{
	//
	// If running realtime, then we submit the request to ibapi
	// Otherwise, we load the ticks from the file and return the 
	// data to the callback function using a thread
	//
	if (realtime)
	{
		std::cout << "rt" << std::endl;
		ibapi->requestRealTimeTicks(*ticker, callback);
	}
	else
	{
		std::cout << "from file" << std::endl;
		readTickDataThread = new std::thread([&] {readTickFile(callback); });
	}
}

void TheTradingMachine::readTickFile(std::function<void(const Tick&tick)> callback)
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
	while (std::getline(*tickDataFile, currLine))
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
		callback(callbackTick);
	}
	std::cout << "done reading from file" << std::endl;
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
