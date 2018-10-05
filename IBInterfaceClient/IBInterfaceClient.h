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

	void requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback);
	bool isReady(void);

private:
	class IBInterfaceClientImpl;
	IBInterfaceClientImpl* impl_;
};

IBINTERFACECLIENTDLL std::shared_ptr<IBInterfaceClient> GetIBInterfaceClient();