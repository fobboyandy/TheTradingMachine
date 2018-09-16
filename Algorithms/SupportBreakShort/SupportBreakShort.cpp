#include <memory>
#include <windows.h>
#include "SupportBreakShort.h"

static std::vector<std::unique_ptr<SupportBreakShort>> SbsInsts;

SupportBreakShort::SupportBreakShort(std::string input, IBInterfaceClient* ibInst) :
	TheTradingMachine(input, ibInst),
	minuteBarMaker(60),
	prevDir(UNDEFINED),
	previousStrength(NONE),
	profit(0)
{

	plotData = new SupportBreakShortPlotData::PlotData;
	plotData->ticks = new std::vector<Tick>;
	plotData->action = new std::vector<std::string>;

	//
	// Register callback functions for new ticks.
	//
	requestTicks([this](const Tick& tick) {this->tickHandler(tick); });

}

SupportBreakShort::~SupportBreakShort()
{
}

void SupportBreakShort::tickHandler(const Tick & tick)
{
	//test by simply exporting the plot data
	plotData->ticks->push_back(tick);


	//Bar minuteBar;
	//lastPrice = tick.price;
	//lastTick = tick;

	////
	//// close all positions
	////
	//if (tick.time % NUM_SECONDS_DAY >= RTH_END)
	//{
	//	for (auto it = openPositions.begin(); it != openPositions.end();)
	//	{
	//		profit += (it->entry - lastPrice) * it->size;
	//		it = openPositions.erase(it);
	//		std::cout << "closed position. profit = " << profit << std::endl;
	//	}
	//	return;
	//}


	////
	//// Check if any positions need to be closed. Ideally this should be handled 
	//// by the broker by sending conditions but we need this here to be able to 
	//// run backtests.
	////
	////coverTrade();

	//static double supportBreakPrice;
	//static bool shorted = false;
	//if (tick.price < prevSupportBar.close && !shorted)
	//{
	//	supportBreakPrice = tick.price;
	//	std::cout << "broke support at " << tick.price << " time " << tick.time << std::endl;
	//}

	////
	//// Enter new trade for closing prices (new candles)
	////
	//if (minuteBarMaker.getRthCandle(tick, minuteBar))
	//{
	//	//
	//	// Check how many support breach end up in candle bar closing under the support
	//	//
	//	static double maxPotential = 0;
	//	if (shorted)
	//	{
	//		maxPotential += supportBreakPrice - minuteBar.close;
	//		std::cout << "max potential : " << maxPotential << std::endl;
	//	}
	//	//
	//	// price increase, check previous direction and mark 
	//	// indicators
	//	//
	//	if (minuteBar.close > prevBar.close)
	//	{
	//		if (prevDir == DOWN)
	//		{
	//			previousStrength = SUPPORT;
	//			prevSupportBar = prevBar;
	//			shorted = false;

	//		}
	//		prevDir = UP;
	//	}
	//	//
	//	// Price decrease. Check if short
	//	//
	//	else if (minuteBar.close < prevBar.close)
	//	{
	//		if (prevDir == UP)
	//		{
	//			previousStrength = RESISTANCE;
	//			prevResistanceBar = prevBar;
	//		}
	//		prevDir = DOWN;
	//		//shortTrade();
	//	}
	//	prevBar = minuteBar;
	//}
}
//
// Check the openPositions for the top most position. The positions are 
// sorted by target prices since the highest target price are the positions
// that need to be closed first. 
//
void SupportBreakShort::coverTrade()
{
	for (auto it = openPositions.begin(); it != openPositions.end(); )
	{
		if (lastPrice == it->target)
		{
			profit += (it->entry - lastPrice) * it->size;
			it = openPositions.erase(it);
			std::cout << "hit target. profit = " << profit << std::endl;
		}
		else if (lastPrice == it->stoploss)
		{
			profit += (it->entry - lastPrice) * it->size;
			it = openPositions.erase(it);
			std::cout << "hit stoploss. profit = " << profit << std::endl;
		}
		else
		{
			it++;
		}
	}
}

void SupportBreakShort::shortTrade()
{
	//
	// If the price falls past the previous support and the risk reward is still worth it,
	// size and short the stock
	//
	if (lastPrice < prevSupportBar.close)
	{
		double targetGain = prevResistanceBar.close - prevSupportBar.close;
		double error = lastPrice - prevSupportBar.close;
		double toleranceFactor = .3;

		if (abs(error) < abs(targetGain * toleranceFactor))
		{
			double targetPrice = prevSupportBar.close - targetGain;
			double stoploss = prevResistanceBar.close;

			Position newPosition;
			newPosition.entry = lastPrice;
			newPosition.stoploss = stoploss;
			newPosition.target = targetPrice;
			newPosition.size = static_cast<int>(100 / abs(newPosition.entry - newPosition.stoploss));
			newPosition.entryTime = lastTick.time;
			openPositions.push_back(newPosition);

			if (openPositions.size() > 1)
			{
				std::cout << "nested positions" << std::endl;
			}


			std::cout << "new short " << std::endl;
			std::cout << "entry: " << newPosition.entry << std::endl;
			std::cout << "time: " << lastTick.time << std::endl;
			std::cout << "size: " << newPosition.size << std::endl;
			std::cout << "previous support: " << prevSupportBar.close << std::endl;
			std::cout << "target: " << newPosition.target << std::endl;
			std::cout << "stoploss: " << newPosition.stoploss << std::endl << std::endl;

		}
	}
}

// int represents a handle to the instantiation of the algorithm corresponding to the input ticker
// this handle needs to be stored by the caller for destruction and calling algorithm
// specific functions. This is necessary because multiple tickers can be running on the same
// algorithm and we only have a single instance of the dll file
int InitAlgorithm(std::string dataInput, IBInterfaceClient * ibInst)
{
	// each time we initialize an algorithm, the size increases by 1
	// the size is returned as a handle to the call for future use
	SbsInsts.push_back(std::unique_ptr<SupportBreakShort>(new SupportBreakShort(dataInput, ibInst)));
	return static_cast<int>(SbsInsts.size() - 1);
}

bool GetPlotData(size_t instHandle, SupportBreakShortPlotData::PlotData** dataOut)
{
	try
	{
		*dataOut = SbsInsts.at(instHandle)->plotData;
		return true;
	}
	catch (const std::out_of_range& oor)
	{
		*dataOut = nullptr;
		UNREFERENCED_PARAMETER(oor);
	}

	return false;
}

bool CloseAlgorithm(size_t instHandle)
{
	try 
	{
		SbsInsts.at(instHandle).reset();
		return true;
	}
	catch (const std::out_of_range& oor)
	{
		UNREFERENCED_PARAMETER(oor);
		//nothing
	}
	return false;

}