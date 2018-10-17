#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <queue>
#include <vector>
#include "../IBInterface/Tick.h"
#include "../IBInterface/bar.h"
#include "../IBInterfaceClient/IBInterfaceClient.h"

#ifdef EXPORTTHETRADINGMACHINEDLL
#define THETRADINGMACHINEDLL __declspec(dllexport)
#else
#define THETRADINGMACHINEDLL __declspec(dllimport)

#define EXPORT_ALGORITHM(CLASSNAME) 																				\
/*boilerplate code for runtime dll linking*/																		\
static std::vector<std::unique_ptr<CLASSNAME>> AlgorithmInstances; 													\
extern "C" 																											\
{ 																													\
	/* 																												\
	* int represents a handle to the instantiation of the algorithm corresponding to the input ticker  				\
	* this handle needs to be stored by the caller for destruction and calling algorithm 							\
	* specific functions. This is necessary because multiple tickers can be running on the same 					\
	* algorithm and we only have a single instance of the dll file 													\
	*/ 																												\
	__declspec(dllexport) int PlayAlgorithm(std::string dataInput, std::shared_ptr<IBInterfaceClient> ibInst) 		\
	{ 																												\
		/* each time we initialize an algorithm, the size increases by 1 											\
		 * the size is returned as a handle to the call for future use  											\
		*/ 																							                \
		auto newInstance = std::make_unique<CLASSNAME>(dataInput, ibInst);										    \
		if (newInstance->valid())																					\
		{																											\
			AlgorithmInstances.push_back(std::move(newInstance));                                                   \
			return static_cast<int>(AlgorithmInstances.size() - 1);													\
		}																											\
		else																										\
		{																											\
			return -1;																								\
		}																											\
	} 																												\
	__declspec(dllexport) bool GetPlotData(int instHandle, std::shared_ptr<PlotData>* dataOut) 						\
	{																												\
		if(instHandle == -1)																						\
		{																											\
			return false;																							\
		}																											\
		try																											\
		{																											\
			*dataOut = AlgorithmInstances.at(static_cast<size_t>(instHandle))->getPlotPlotData();					\
			return (*dataOut != nullptr);																			\
		}																											\
		catch (const std::out_of_range& oor)																		\
		{																											\
			/* don't need to set dataOut to nullptr. let the caller decide from the return status */			    \
			UNREFERENCED_PARAMETER(oor);																			\
		}																											\
																													\
		return false;																								\
	}																												\
																													\
	__declspec(dllexport) bool StopAlgorithm(int instHandle)														\
	{																												\
		if(instHandle == -1)																						\
		{																										    \
			/*do nothing return true since stopping was successful nevertheless*/									\
			return true;																						    \
		}																										    \
		try																											\
		{																											\
			AlgorithmInstances.at(instHandle).reset();																\
			return true;																							\
		}																											\
		catch (const std::out_of_range& oor)																		\
		{																											\
			UNREFERENCED_PARAMETER(oor);																			\
			/*nothing*/																								\
		}																											\
																													\
		return false;																								\
	}																												\
}																													\

#endif

// plot data structure shared with the gui. plot data is stored in this format.
// the gui is provided an address to the plot data struct and notified upon 
// new data
struct PlotData
{
	std::mutex plotDataMtx;
	bool finished;
	std::queue<Tick> buffer;
	std::vector<Tick> ticks;
	std::vector<std::string> action;
};

class THETRADINGMACHINEDLL TheTradingMachine
{
public:
	explicit TheTradingMachine(std::string in, std::shared_ptr<IBInterfaceClient> ibApiPtr = std::shared_ptr<IBInterfaceClient>(nullptr));

	virtual ~TheTradingMachine();
	std::shared_ptr<PlotData> getPlotPlotData();
	bool valid() const;

private:
	class TheTradingMachineImpl;
	TheTradingMachineImpl* impl_;
protected:
	// we need the start and stop functions because tickHandler is pure virtual.
	// because TheTradingMachine starts a thread that runs on the derived classes'
	// tickHandler, we need the derived class to be able to stop our thread first
	// before their tickHandler goes out of scope (derived classes get destructed
	// before base classes) to prevent the running thread to access the function
	// of the destructed derived class
	void start(void);
	void stop(void);
	virtual void tickHandler(const Tick& tick) = 0;
	
	//let the impl class call the pure virtual tickHandler
	friend TheTradingMachineImpl;
};
