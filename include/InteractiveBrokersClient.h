#pragma once

#include <string>
#include <functional>
#include <memory>
#include "../external/InteractiveBrokersApi/Tick.h"

#ifdef InteractiveBrokersClient_EXPORTS
#define INTERACTIVEBROKERSCLIENTDLL __declspec(dllexport)
#else
#define INTERACTIVEBROKERSCLIENTDLL __declspec(dllimport)
#endif

class INTERACTIVEBROKERSCLIENTDLL InteractiveBrokersClient
{
public:
	InteractiveBrokersClient();
	~InteractiveBrokersClient();

	//order api
	int longMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification);
	int longLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification);
	int shortMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification);
	int shortLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification);

	int requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback);
	void cancelRealTimeTicks(std::string ticker, int handle);
	bool isReady(void);

	void unregisterFillNotification(int handle);

private:

	class InteractiveBrokersClientImpl;
	InteractiveBrokersClientImpl* impl_;
};

INTERACTIVEBROKERSCLIENTDLL std::shared_ptr<InteractiveBrokersClient> GetInteractiveBrokersClient();
