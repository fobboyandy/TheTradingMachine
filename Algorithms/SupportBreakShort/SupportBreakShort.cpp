#include <memory>
#include "SupportBreakShort.h"
#include "../../BaseAlgorithm/BaseAlgorithm/Annotation.h"

SupportBreakShort::SupportBreakShort(std::string input, std::shared_ptr<InteractiveBrokersClient> ibInst, bool live) :
	BaseAlgorithm(input, ibInst, live),
	input(input),
	prevDir(UNDEFINED),
	previousStrength(NONE)
{
}

SupportBreakShort::~SupportBreakShort()
{
}

void SupportBreakShort::tickHandler(const Tick & tick)
{

	static bool fired = false;

	if (!fired)
	{
		fired = true;
		auto plotData = getPlotData();

		//straight line from first candle to two minutes later
		auto sampleLabel = std::make_shared<Annotation::Label>("asdfdf", tick.time, tick.price);
		auto sampleLine = std::make_shared<Annotation::Line>(tick.time, 1000, tick.time + 12000, 400000);
		plotData->annotations.push_back(sampleLabel);
		plotData->annotations.push_back(sampleLine);
	}

	// does nothing for now. all plot data is handled in base class

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

EXPORT_ALGORITHM(SupportBreakShort)