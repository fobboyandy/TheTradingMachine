#pragma once

#include <string>
#include "Common.h"
#include "PlotData.h"

#ifdef BASEALGORITHM_EXPORTS
#define BASEALGORITHMDLL __declspec(dllexport)
#else
#define BASEALGORITHMDLL __declspec(dllimport)
#include <unordered_map>
#include <memory>
#include <stdexcept>

#define EXPORT_ALGORITHM(CLASSNAME) 																				\
/*boilerplate code for runtime dll linking*/																		\
static std::unordered_map<int, std::unique_ptr<CLASSNAME>> AlgorithmInstances; 										\
extern "C" 																											\
{ 																													\
	/* 																												\
	* int represents a handle to the instantiation of the algorithm corresponding to the input ticker  				\
	* this handle needs to be stored by the caller for destruction and calling algorithm 							\
	* specific functions. This is necessary because multiple tickers can be running on the same 					\
	* algorithm and we only have a single instance of the dll file 													\
	*/ 																												\
	__declspec(dllexport) int PlayAlgorithm(																		\
		std::string dataInput,																						\
		std::shared_ptr<PlotData>* dataOut,																			\
		std::shared_ptr<InteractiveBrokersClient> ibInst,															\
		bool live) 																									\
	{ 																												\
		return PlayAlgorithmT<CLASSNAME>(dataInput, dataOut, ibInst, live);											\
	} 																												\
																													\
	__declspec(dllexport) bool StopAlgorithm(int instHandle) 														\
	{ 																												\
		return StopAlgorithmT<CLASSNAME>(instHandle);																\
	} 																												\
}

// not really sure how to define these template functions outside of a macro
template<class Algorithm>
__declspec(dllexport) int PlayAlgorithmT(
	std::string dataInput,
	std::shared_ptr<PlotData>* dataOut,
	std::shared_ptr<InteractiveBrokersClient> ibInst,
	bool live)
{
	static int uniqueInstanceHandles = 0;
	try
	{
		auto newInstance = std::make_unique<Algorithm>(dataInput, ibInst, live);
		*dataOut = newInstance->getPlotData();
		newInstance->run();
		AlgorithmInstances[uniqueInstanceHandles] = std::move(newInstance);
		return static_cast<int>(uniqueInstanceHandles++);
	}
	catch (const std::runtime_error&)
	{
		return -1;
	}
}

template<class Algorithm>
__declspec(dllexport) bool StopAlgorithmT(int instHandle)
{
	if (AlgorithmInstances.find(instHandle) != AlgorithmInstances.end())
	{
		AlgorithmInstances[instHandle]->stop();
		AlgorithmInstances.erase(instHandle);
	}
	return true;
}
#endif

class BASEALGORITHMDLL BaseAlgorithm
{
public:
	BaseAlgorithm(std::string input, std::shared_ptr<InteractiveBrokersClient> ibApiPtr = std::shared_ptr<InteractiveBrokersClient>(nullptr), bool live = false);

	virtual ~BaseAlgorithm();
	std::shared_ptr<PlotData> getPlotData();

	// not meant to be overridden
	virtual void run() final;
	virtual void stop() final;

//ordering api
public:
	PositionId longMarketNoStop(std::string ticker, int numShares);
	PositionId longMarketStopMarket(std::string ticker, int numShares, double stopPrice);
	PositionId longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	PositionId shortMarketNoStop(std::string ticker, int numShares);
	PositionId shortMarketStopMarket(std::string ticker, int numShares, double activationPrice);
	PositionId shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	void closePosition(PositionId posId);
	void reducePosition(PositionId posId, int numShares);
	Position getPosition(PositionId posId);

	virtual void tickHandler(const Tick& tick) = 0;
private:
	class BaseAlgorithmImpl;
	BaseAlgorithmImpl* _impl;
protected:
	
	//let the impl class call the pure virtual tickHandler
	friend BaseAlgorithmImpl;
};
