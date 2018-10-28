#pragma once

#include <string>
#include "Common.h"
#include "PlotData.h"
#include "../IBInterfaceClient/IBInterfaceClient.h"

#ifdef EXPORTTHETRADINGMACHINEDLL
#define THETRADINGMACHINEDLL __declspec(dllexport)
#else
#define THETRADINGMACHINEDLL __declspec(dllimport)
#include <unordered_map>
#include <memory>
#include <stdexcept>

#define EXPORT_ALGORITHM(CLASSNAME) 																				\
/*boilerplate code for runtime dll linking*/																		\
static int uniqueInstanceHandles = 0;																				\
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
		std::shared_ptr<IBInterfaceClient> ibInst,																	\
		bool live) 																									\
	{ 																												\
		try																											\
		{																											\
			auto newInstance = std::make_unique<CLASSNAME>(dataInput, ibInst, live);								\
			*dataOut = newInstance->getPlotData();																	\
			AlgorithmInstances[uniqueInstanceHandles] = std::move(newInstance);										\
			return static_cast<int>(uniqueInstanceHandles++);														\
		}																											\
		catch (const std::runtime_error&)																			\
		{																											\
			return -1;																								\
		}																											\
	} 																												\
																													\
	__declspec(dllexport) bool StopAlgorithm(int instHandle)														\
	{																												\
																													\
		AlgorithmInstances.erase(instHandle);																		\
		return true;																								\
	}																												\
}
#endif

class THETRADINGMACHINEDLL TheTradingMachine
{
public:
	TheTradingMachine(std::string input, std::shared_ptr<IBInterfaceClient> ibApiPtr = std::shared_ptr<IBInterfaceClient>(nullptr), bool live = false);

	virtual ~TheTradingMachine();
	std::shared_ptr<PlotData> getPlotData();
	void run();
	void stop();

private:
	class TheTradingMachineImpl;
	TheTradingMachineImpl* _impl;
protected:
	virtual void tickHandler(const Tick& tick) = 0;
	
	//let the impl class call the pure virtual tickHandler
	friend TheTradingMachineImpl;
};
