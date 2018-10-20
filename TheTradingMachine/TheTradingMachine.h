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
	__declspec(dllexport) int PlayAlgorithm(	                                                                    \
			std::string dataInput,				                                                                    \
			std::shared_ptr<PlotData>* dataOut,                                                                     \
			std::shared_ptr<IBInterfaceClient> ibInst) 																\
	{ 																												\
		/* each time we initialize an algorithm, the size increases by 1 											\
		 * the size is returned as a handle to the call for future use  											\
		*/ 																							                \
		auto newInstance = std::make_unique<CLASSNAME>(dataInput, ibInst);										    \
		if (newInstance->engine.valid())																			\
		{																											\
			if(dataOut != nullptr)																					\
			{																										\
				*dataOut = newInstance->engine.getPlotData();											     		\
			}																										\
			AlgorithmInstances.push_back(std::move(newInstance));													\
																													\
			return static_cast<int>(AlgorithmInstances.size() - 1);													\
		}																											\
		else																										\
		{																											\
			return -1;																								\
		}																											\
	} 																												\
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

#define THETRADINGMACHINE_OBJ		\
	public:							\
		TheTradingMachine engine;	\
	private:
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
	explicit TheTradingMachine(std::string in, std::function<void(const Tick&)> algTickCallback, std::shared_ptr<IBInterfaceClient> ibApiPtr = std::shared_ptr<IBInterfaceClient>(nullptr));

	virtual ~TheTradingMachine();
	std::shared_ptr<PlotData> getPlotData();
	bool valid() const;

private:
	class TheTradingMachineImpl;
	TheTradingMachineImpl* impl_;
};
