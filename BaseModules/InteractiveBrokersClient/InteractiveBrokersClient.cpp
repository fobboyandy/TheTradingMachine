#include <thread>
#include <unordered_map>
#include <iostream>
#include "InteractiveBrokersClient.h"
#include "../InteractiveBrokersApi/InteractiveBrokersApi.h"
#include "../InteractiveBrokersApi/CommonDefs.h"

class InteractiveBrokersClient::InteractiveBrokersClientImpl
{
public:
	InteractiveBrokersClientImpl();
	~InteractiveBrokersClientImpl();
	InteractiveBrokersClientImpl(const InteractiveBrokersClientImpl& other) = delete;
	InteractiveBrokersClientImpl(InteractiveBrokersClientImpl&& other) = delete;

	void longMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification);
	void longLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification);
	void shortMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification);
	void shortLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification);

	int requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback);
	bool cancelRealTimeTicks(std::string ticker, int handle);

	// 
	// This function is registered to the ibApi's actual requestRealTimeTicks. This function
	// will be called with an order id as the input and the new tick that came in. tickDispatcher
	// will then look for all the callbacks with the associated orderId and dispatch the tick
	// to all the callbacks with the new tick.
	//
	void tickDispatcher(OrderId oid, const Tick& tick);
	bool isReady(void);

private:
	int callbackHandle; // used to uniquely create a handle each time requestRealTimeTicks is called
	InteractiveBrokersApi ibApi;
	std::thread messageProcessThread;
	std::atomic<bool> threadCancellationToken;
	void messageProcessThreadFn(void);

// members to handle routing data requests to the appropriate callback
private:
	// Tick Data
	// A call back function can be uniquely identified given a ticker and a handle
	std::unordered_map<std::string, OrderId> tickerOrderIds;
	std::unordered_map<OrderId, std::unordered_map<int, std::function<void(const Tick&)>>> tickCallbacks;
	std::mutex tickCallbacksMtx; //synchronizes message processing thread and request and cancel functions
};

InteractiveBrokersClient::InteractiveBrokersClientImpl::InteractiveBrokersClientImpl():
	threadCancellationToken(false)
{
	callbackHandle = 0;
	std::cout << "Initializing IB Client" << std::endl;

	// if connection is established, then thread becomes the message processing thread.
	messageProcessThread = std::thread([this]()
	{
		int attempts = 0;
		const int MAX_ATTEMPTS = 50;
		while (!threadCancellationToken.load() && !ibApi.isConnected() && attempts < MAX_ATTEMPTS)
		{
			ibApi.connect("", 7496, 0);
			std::cout << "Connect attempt " << attempts << std::endl;
			Sleep(1000);
			attempts++;
		}

		if (attempts < MAX_ATTEMPTS)
		{
			std::cout << "Initialized IB Client. Message processing started..." << std::endl;

			//register our tick data dispatcher
			ibApi.registerRealTimeTickCallback([this](OrderId oid, const Tick& tick) {this->tickDispatcher(oid, tick); });
			messageProcessThreadFn();
		}
		else
		{
			std::cout << "Reached maximum number of attempts to connect."<< std::endl;
		}
	});
}

InteractiveBrokersClient::InteractiveBrokersClientImpl::~InteractiveBrokersClientImpl()
{
	threadCancellationToken = true;

	std::cout << "impl destruct" << std::endl;
	if (messageProcessThread.joinable())
	{
		messageProcessThread.join();
	}
	if (ibApi.isConnected())
	{
		ibApi.disconnect();
	}
}

void InteractiveBrokersClient::InteractiveBrokersClientImpl::longMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification)
{
}

void InteractiveBrokersClient::InteractiveBrokersClientImpl::longLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification)
{
}

void InteractiveBrokersClient::InteractiveBrokersClientImpl::shortMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification)
{
}

void InteractiveBrokersClient::InteractiveBrokersClientImpl::shortLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification)
{
}

// requests from ib api for real time ticks. When real time ticks come in, callback will be called with the Tick data
// as the input. requestRealTimeTicks from ib api will return an order Id. We must keep the mapping of this order id to the
// appropriate ticker name and list of callbacks in order to call the proper callback functions.
int InteractiveBrokersClient::InteractiveBrokersClientImpl::requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback)
{
	if (ibApi.isReady())
	{
		OrderId oid;
		// if this ticker already has an order id, then a data stream already has been
		// requested before. otherwise, request a new data stream and assign the
		// orderid to the ticker
		if (tickerOrderIds.find(ticker) != tickerOrderIds.end())
		{
			oid = tickerOrderIds[ticker];
		}
		else
		{
			oid = ibApi.requestRealTimeTicks(ticker);
			tickerOrderIds[ticker] = oid;
		}

		std::lock_guard<std::mutex> lock(tickCallbacksMtx);
		tickCallbacks[oid].insert(std::pair<int, std::function<void(const Tick&)>>(callbackHandle, callback));
		
		// return current value of handle and increment after
		return callbackHandle++;
	}

	return -1;
}

bool InteractiveBrokersClient::InteractiveBrokersClientImpl::cancelRealTimeTicks(std::string ticker, int handle)
{
	// check if the provided ticker has an associated order id. can't cancel 
	// if it doesn't exist
	if (ibApi.isReady() && tickerOrderIds.find(ticker) != tickerOrderIds.end())
	{
		auto oid = tickerOrderIds[ticker];
		if(tickCallbacks.find(oid) != tickCallbacks.end())
		{		
			// read and modify entries under a lock because 
			// another thread is accessing the functions
			std::lock_guard<std::mutex> lock(tickCallbacksMtx);

			// erase doesn't throw exceptions
			tickCallbacks[oid].erase(handle);

			// if there are no more callbacks associated with the orderId,
			// delete the entry
			if (tickCallbacks[oid].size() == 0)
			{
				ibApi.cancelRealTimeTicks(oid);
				// For now, no success checking of cancelTickbyTickData before removing
				tickCallbacks.erase(oid);
				tickerOrderIds.erase(ticker);
			}
			return true;
		}
	}
	return false;
}

void InteractiveBrokersClient::InteractiveBrokersClientImpl::tickDispatcher(OrderId oid, const Tick & tick)
{
	// call all the associated functions under the lock because another thread
	// might be modifying the tickCallbacks 
	std::lock_guard<std::mutex> lockGuard(tickCallbacksMtx);
	for (const auto& fn : tickCallbacks[oid])
	{
		fn.second(tick);
	}
	
}

bool InteractiveBrokersClient::InteractiveBrokersClientImpl::isReady(void)
{	
	return ibApi.isReady();
}

void InteractiveBrokersClient::InteractiveBrokersClientImpl::messageProcessThreadFn(void)
{
	//shouldnt need to check for nulls here because we can only get here if these are not nullptr
	while (ibApi.isConnected() && !threadCancellationToken)
	{
		ibApi.processMessages();
		Sleep(10);
	}
}

InteractiveBrokersClient::InteractiveBrokersClient() :
	impl_(new InteractiveBrokersClientImpl)
{

}

InteractiveBrokersClient::~InteractiveBrokersClient()
{
	if (impl_ != nullptr)
	{
		delete impl_;
		impl_ = nullptr;
	}
}

void InteractiveBrokersClient::longMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification)
{
	impl_->longMarket(ticker, numShares, fillNotification);
}

void InteractiveBrokersClient::longLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification)
{
	impl_->longLimit(ticker, limitPrice, numShares, fillNotification);
}

void InteractiveBrokersClient::shortMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification)
{
	impl_->shortMarket(ticker, numShares, fillNotification);
}

void InteractiveBrokersClient::shortLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification)
{
	impl_->shortLimit(ticker, limitPrice, numShares, fillNotification);
}

int InteractiveBrokersClient::requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback)
{
	return impl_->requestRealTimeTicks(ticker, callback);
}

bool InteractiveBrokersClient::cancelRealTimeTicks(std::string ticker, int handle)
{
	return impl_->cancelRealTimeTicks(ticker, handle);
}

bool InteractiveBrokersClient::isReady(void)
{
	return impl_->isReady();
}

INTERACTIVEBROKERSCLIENTDLL std::shared_ptr<InteractiveBrokersClient> GetInteractiveBrokersClient()
{
	static auto ibInterfaceInst = std::make_shared<InteractiveBrokersClient>();
	return ibInterfaceInst;
}