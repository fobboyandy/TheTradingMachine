#pragma once

#include <string>
#include <memory>
#include <functional>

#ifdef IBINTERFACECLIENT_EXPORTS
#define IBINTERFACECLIENTDLL __declspec(dllexport)
#include "../IBInterface/IBInterface.h"
#else
#define IBINTERFACECLIENTDLL __declspec(dllimport)
#include "../IBInterface/Tick.h"
#endif

class IBINTERFACECLIENTDLL IBInterfaceClient
{
public:
	IBInterfaceClient();
	~IBInterfaceClient();

	int requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback);
	bool cancelRealTimeTicks(std::string ticker, int handle);
	bool isReady(void);

private:

	class IBInterfaceClientImpl;
	IBInterfaceClientImpl* impl_;
};

IBINTERFACECLIENTDLL std::shared_ptr<IBInterfaceClient> GetIBInterfaceClient();
