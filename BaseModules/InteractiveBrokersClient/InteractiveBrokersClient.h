#pragma once

#include <string>
#include <functional>
#include "../InteractiveBrokersApi/Tick.h"

#ifdef INTERACTIVEBROKERSCLIENT_EXPORTS
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
	void longMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification);
	void longLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification);
	void shortMarket(std::string ticker, int numShares, std::function<void(double, time_t)> fillNotification);
	void shortLimit(std::string ticker, double limitPrice, int numShares, std::function<void(double, time_t)> fillNotification);

	int requestRealTimeTicks(std::string ticker, std::function<void(const Tick&)> callback);
	bool cancelRealTimeTicks(std::string ticker, int handle);
	bool isReady(void);

private:

	class InteractiveBrokersClientImpl;
	InteractiveBrokersClientImpl* impl_;
};

INTERACTIVEBROKERSCLIENTDLL std::shared_ptr<InteractiveBrokersClient> GetInteractiveBrokersClient();
