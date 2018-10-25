#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <queue>
#include <vector>
#include "../IBInterface/Tick.h"
#include "../IBInterface/bar.h"
#include "../IBInterfaceClient/IBInterfaceClient.h"
#include "Portfolio.h"

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
		/*always keep using the last element of the vector as the handle to the algorithm*/							\
		static auto handles = 0;																					\
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
			return static_cast<int>(handles++);																		\
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
			/*nothing for now*/																						\
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

// Order api
public:
	// When these order functions are called, a PositionId is returned immediately. The functions
	// do not block until the positions are filled. Since it's non blocking, the position is not guaranteed
	// to be filled when the function returns. With the positionId however, the caller can query the status of the position
	// using getPosition(PositionId).
	PositionId buyMarketNoStop(std::string ticker);
	PositionId buyMarketStopMarket(std::string ticker, double stopPrice);
	PositionId buyMarketStopLimit(std::string ticker, double activationPrice, double limitPrice);
	PositionId buyLimitStopMarket(std::string ticker, double buyLimit, double activationPrice);
	PositionId buyLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice);

	PositionId sellMarketNoStop(std::string ticker);
	PositionId sellMarketStopMarket(std::string ticker, double activationPrice);
	PositionId sellMarketStopLimit(std::string ticker, double activationPrice, double limitPrice);
	PositionId sellLimitStopMarket(std::string ticker, double buyLimit, double activationPrice);
	PositionId sellLimitStopLimit(std::string ticker, double buyLimit, double activationPrice, double limitPrice);

	// getPosition gets the current state of a position. It returns a copy to the caller.
	Position getPosition(PositionId posId);

	// modifyPosition allows a user to update an existing position if the update is on the same side.
	// for example. a short can only increase or decrease but cannot be turned into a long
	void modifyPosition(PositionId posId, Position newPosition);

	// closes an existing position. It guarantees that an existing position will not be overbought/sold due to a stoploss attached to an order.
	void closePosition(PositionId posId);
};
