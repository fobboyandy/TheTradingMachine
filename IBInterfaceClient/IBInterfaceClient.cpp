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

	void impl_requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback);
	bool impl_isReady(void);
private:
	IBInterface impl_ibapi;
	std::thread impl_messageProcessThread;
	std::atomic<bool> impl_threadCancellationToken;
	void impl_messageProcessThreadFn(void);
};

IBInterfaceClient::IBInterfaceClientImpl::IBInterfaceClientImpl():
	impl_threadCancellationToken(false)
{
	//std::cout << "Initializing IB Client" << std::endl;
	//if (client != nullptr)
	//{
	//	//connect to ib api
	//	if (client->Initialize())
	//	{
	//		messageProcess_ = new std::thread(&IBInterfaceClient::messageProcess, this);
	//		//
	//		// Wait for message handling thread to initialize
	//		//
	//		while (!runThread->load())
	//		{
	//			Sleep(10);
	//		}
	//		std::cout << "Successfully initialized IB Client." << std::endl;

	//	}
	//	else
	//	{
	//		std::cout << "Unable to connect to neither TWS nor IB Gateway!" << std::endl;
	//		delete client;
	//		client = nullptr;
	//		Sleep(10000);
	//	}
	//}

}

IBInterfaceClient::IBInterfaceClientImpl::~IBInterfaceClientImpl()
{
	//if (runThread != nullptr && runThread->load())
	//{
	//	//
	//	// Stop the thread
	//	//
	//	runThread->store(false);
	//	if (messageProcess_ != nullptr)
	//	{
	//		messageProcess_->join();
	//		delete messageProcess_;
	//		messageProcess_ = nullptr;
	//	}
	//	delete runThread;
	//	runThread = nullptr;
	//}

	//if (client != nullptr)
	//{
	//	client->disconnect();
	//	delete client;
	//	client = nullptr;
	//}
}

void IBInterfaceClient::IBInterfaceClientImpl::impl_requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback)
{
	//if (client != nullptr)
	//{
	//	client->requestRealTimeTicks(ticker, callback);
	//}
}

bool IBInterfaceClient::IBInterfaceClientImpl::impl_isReady(void)
{	
	//// runThread is true when the connection has been initialized and the 
	//// messaging thread has started
	//return runThread->load();
	return false;
}

void IBInterfaceClient::IBInterfaceClientImpl::impl_messageProcessThreadFn(void)
{
	//if (client->isConnected())
	//{
	//	std::cout << "IB Message Processing Thread has started." << std::endl;
	//	runThread->store(true);
	//}
	////shouldnt need to check for nulls here because we can only get here if these are not nullptr
	//while (client->isConnected() && runThread->load())
	//{
	//	client->processMessages();
	//	Sleep(10);
	//}
}

IBInterfaceClient::IBInterfaceClient() :
	impl_(new IBInterfaceClientImpl)
{

}

IBInterfaceClient::~IBInterfaceClient()
{

}

void IBInterfaceClient::requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback)
{

}

bool IBInterfaceClient::isReady(void)
{
	return false;
}

IBINTERFACECLIENTDLL std::shared_ptr<IBInterfaceClient> GetIBInterfaceClient()
{
	static auto ibInterfaceInst = std::make_shared<IBInterfaceClient>(IBInterfaceClient());
	return ibInterfaceInst;
}