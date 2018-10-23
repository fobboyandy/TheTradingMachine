// IBInterfaceClient.cpp : Defines the exported functions for the DLL application.
//

#include <thread>
#include <iostream>
#include "IBInterfaceClient.h"

class IBInterfaceClient::IBInterfaceClientImpl
{
public:
	IBInterfaceClientImpl();
	~IBInterfaceClientImpl();
	IBInterfaceClientImpl(const IBInterfaceClientImpl& other) = delete;
	IBInterfaceClientImpl(IBInterfaceClientImpl&& other) = delete;

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

	// Order functions
	void buyMarketNoStop(std::string ticker);
	void buyMarketStopMarket(std::string ticker, double activationPrice);
	void buyMarketStopLimit(std::string ticker, double activationPrice, double limitPrice);
	void buyLimitStopMarket(std::string ticker, double buyLimit, double activationPrice);
	void buyLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice);

	void sellMarketNoStop(std::string ticker);
	void sellMarketStopMarket(std::string ticker, double activationPrice);
	void sellMarketStopLimit(std::string ticker, double activationPrice, double limitPrice);
	void sellLimitStopMarket(std::string ticker, double buyLimit, double activationPrice);
	void sellLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice);

private:
	IBInterface ibApi;
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

IBInterfaceClient::IBInterfaceClientImpl::IBInterfaceClientImpl():
	threadCancellationToken(false)
{
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

IBInterfaceClient::IBInterfaceClientImpl::~IBInterfaceClientImpl()
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

// requests from ib api for real time ticks. When real time ticks come in, callback will be called with the Tick data
// as the input. requestRealTimeTicks from ib api will return an order Id. We must keep the mapping of this order id to the
// appropriate ticker name and list of callbacks in order to call the proper callback functions.
int IBInterfaceClient::IBInterfaceClientImpl::requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback)
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

		// used to uniquely create a handle each time requestRealTimeTicks is called
		static int handle = 0;
		std::lock_guard<std::mutex> lock(tickCallbacksMtx);
		tickCallbacks[oid].insert(std::pair<int, std::function<void(const Tick&)>>(handle, callback));
		
		// return current value of handle and increment after
		return handle++;
	}

	return -1;
}

bool IBInterfaceClient::IBInterfaceClientImpl::cancelRealTimeTicks(std::string ticker, int handle)
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

void IBInterfaceClient::IBInterfaceClientImpl::tickDispatcher(OrderId oid, const Tick & tick)
{
	// call all the associated functions under the lock because another thread
	// might be modifying the tickCallbacks 
	std::lock_guard<std::mutex> lockGuard(tickCallbacksMtx);
	for (const auto& fn : tickCallbacks[oid])
	{
		fn.second(tick);
	}
	
}

bool IBInterfaceClient::IBInterfaceClientImpl::isReady(void)
{	
	return ibApi.isReady();
}

void IBInterfaceClient::IBInterfaceClientImpl::buyMarketNoStop(std::string ticker)
{
}

void IBInterfaceClient::IBInterfaceClientImpl::buyMarketStopMarket(std::string ticker, double activationPrice)
{
}

void IBInterfaceClient::IBInterfaceClientImpl::buyMarketStopLimit(std::string ticker, double activationPrice, double limitPrice)
{
}

void IBInterfaceClient::IBInterfaceClientImpl::buyLimitStopMarket(std::string ticker, double buyLimit, double activationPrice)
{
}

void IBInterfaceClient::IBInterfaceClientImpl::buyLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice)
{
}

void IBInterfaceClient::IBInterfaceClientImpl::sellMarketNoStop(std::string ticker)
{
}

void IBInterfaceClient::IBInterfaceClientImpl::sellMarketStopMarket(std::string ticker, double activationPrice)
{
}

void IBInterfaceClient::IBInterfaceClientImpl::sellMarketStopLimit(std::string ticker, double activationPrice, double limitPrice)
{
}

void IBInterfaceClient::IBInterfaceClientImpl::sellLimitStopMarket(std::string ticker, double buyLimit, double activationPrice)
{
}

void IBInterfaceClient::IBInterfaceClientImpl::sellLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice)
{
}

void IBInterfaceClient::IBInterfaceClientImpl::messageProcessThreadFn(void)
{
	//shouldnt need to check for nulls here because we can only get here if these are not nullptr
	while (ibApi.isConnected() && !threadCancellationToken)
	{
		ibApi.processMessages();
		Sleep(10);
	}
}

IBInterfaceClient::IBInterfaceClient() :
	impl_(new IBInterfaceClientImpl)
{

}

IBInterfaceClient::~IBInterfaceClient()
{
	if (impl_ != nullptr)
	{
		delete impl_;
		impl_ = nullptr;
	}
}

int IBInterfaceClient::requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback)
{
	return impl_->requestRealTimeTicks(ticker, callback);
}

bool IBInterfaceClient::cancelRealTimeTicks(std::string ticker, int handle)
{
	return impl_->cancelRealTimeTicks(ticker, handle);
}

bool IBInterfaceClient::isReady(void)
{
	return impl_->isReady();
}

IBINTERFACECLIENTDLL std::shared_ptr<IBInterfaceClient> GetIBInterfaceClient()
{
	std::cout << "GetIBInterfaceClient" << std::endl;
	static auto ibInterfaceInst = std::make_shared<IBInterfaceClient>();
	return ibInterfaceInst;
}