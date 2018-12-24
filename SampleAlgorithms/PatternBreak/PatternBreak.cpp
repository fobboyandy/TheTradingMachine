// PatternBreak.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "../../BaseModules/BaseAlgorithm/BaseAlgorithm.h"
#include "../../BaseModules/CandleMaker/CandleMaker.h"
#include "../../BaseModules/Indicators/SimpleMovingAverage.h"
#include "../../BaseModules/Indicators/ExponentialMovingAverage.h"
#include "../../BaseModules/Indicators/BollingerBands.h"

#include <windows.h>


#pragma comment(lib, "../../BaseModules/x64/Debug/BaseAlgorithm.lib")
#pragma comment(lib, "../../BaseModules/x64/Debug/CandleMaker.lib")
#pragma comment(lib, "../../BaseModules/x64/Debug/Indicators.lib")



class PatternBreak : public BaseAlgorithm
{
public:
	PatternBreak(ALGORITHM_ARGS);
	~PatternBreak();

private:
	CandleMaker candleMaker;
	SimpleMovingAverage priceSma;
	SimpleMovingAverage dPriceSmaSma;

	
	std::vector<Candlestick> candles;
	std::vector<double> priceSmaValues;
	std::vector<double> dpriceSmaSmaValues;

	std::list<Candlestick> supportCandles;
	std::list<Candlestick> resistanceCandles;

	int posId = -1;

	// stopping conditions: target hit, stoploss hit, time limit hit
	double stopPrice = -1;
	double targetPrice = -1;
	time_t targetTime = -1;

	void tickHandler(const Tick& tick);

	// finds support and resistance based on updated candles member
	void updateSupportAndResistance(const Candlestick& candle);
};

PatternBreak::PatternBreak(ALGORITHM_ARGS) :
	BaseAlgorithm(BASEALGORITHM_PASS_ARGS),
	priceSma(5),
	dPriceSmaSma(5),
	candleMaker(60)
{
}

PatternBreak::~PatternBreak()
{
}

void PatternBreak::tickHandler(const Tick & tick)
{
	candleMaker.addTick(tick);
	auto closedCandles = candleMaker.getClosedCandles();
	for (auto& candle: closedCandles)
	{
		updateSupportAndResistance(candle);


		// check if any conditions has been hit for the current position
		// we only deal with one position at a time
		if (posId != -1)
		{
			if (tick.price <= targetPrice || tick.time >= targetTime || tick.price >= stopPrice)
			{
				closePosition(posId);
				posId = -1;
			}
		}
		// support/resistanceCandles get cleared and repopulated with the latest
		// support/resistance. We use empty to check whether there is one
		else if (!supportCandles.empty())
		{
			auto candle = supportCandles.front();
			if (tick.price < candle.close)
			{
				posId = shortMarketNoStop(ticker(), 100);
				// set stopping conditions

				targetPrice = tick.price - 1;
				targetTime = tick.time + 180;
				stopPrice = .999 * tick.price;
				supportCandles.clear();
			}
		}


	}
}

void PatternBreak::updateSupportAndResistance(const Candlestick& candle)
{
	candles.push_back(candle);
	priceSmaValues.emplace_back(priceSma.computeIndicatorPoint(candle.close)[SimpleMovingAverage::MOVING_AVERAGE]);

	// needs two or more points to start calculating slopes
	if (priceSmaValues.size() >= 2)
	{
		const auto dPrice = priceSmaValues.back() - priceSmaValues[priceSmaValues.size() - 2];
		dpriceSmaSmaValues.emplace_back(dPriceSmaSma.computeIndicatorPoint(dPrice)[SimpleMovingAverage::MOVING_AVERAGE]);
		if (dpriceSmaSmaValues.size() >= 2)
		{
			const auto& currCandle = candles.back();
			const auto& prevCandle = candles[candles.size() - 2];

			const auto& prevdPriceSma = dpriceSmaSmaValues[dpriceSmaSmaValues.size() - 2];
			const auto& currdPriceSma = dpriceSmaSmaValues.back();

			// multiplying values by 5 to increase visibility. the values
			// don't matter anyway since we only care about zero crossings
			auto line = std::make_shared<Annotation::Line>(prevCandle.time, prevdPriceSma * 5, currCandle.time, currdPriceSma * 5, 2);
			line->color_ = { 255, 0, 0 }; //red

			getPlotData()->annotations.push_back(line);
			// zero crossings
			if (prevdPriceSma < 0 && currdPriceSma > 0)
			{
				// this is a local min, look for the support
				decltype(candles.size()) count = 0;
				auto rIt = candles.rbegin();
				auto minCandle = currCandle;
				for (; rIt != candles.rend() && count < 10; ++rIt, ++count)
				{
					if (rIt->low < minCandle.low)
					{
						minCandle = *rIt;
					}
				}

				double price = 0;
				// green candle. use open as support level
				if (minCandle.close > minCandle.open)
				{
					price = minCandle.open;
				}
				else
				{
					price = minCandle.close;
				}
				
				supportCandles.clear();
				supportCandles.push_back(minCandle);

				auto box = std::make_shared<Annotation::Box>(minCandle.time - 300, 1.001 * price, minCandle.time + 300, .999 * price, 0);
				box->color_ = { 0, 255, 0 }; //green for supports
				getPlotData()->annotations.push_back(box);

			}
			else if (prevdPriceSma > 0 && currdPriceSma < 0)
			{
				// this is a local max, look for the resistance
				decltype(candles.size()) count = 0;
				auto rIt = candles.rbegin();
				auto maxCandle = currCandle;
				for (; rIt != candles.rend() && count < 10; ++rIt, ++count)
				{
					if (rIt->high > maxCandle.high)
					{
						maxCandle = *rIt;
					}
				}

				double price = 0;
				// green candle. use close as resistance level
				if (maxCandle.close > maxCandle.open)
				{
					price = maxCandle.close;
				}
				else
				{
					price = maxCandle.open;
				}

				resistanceCandles.clear();
				resistanceCandles.push_back(maxCandle);

				auto box = std::make_shared<Annotation::Box>(maxCandle.time - 300, 1.001 * price, maxCandle.time + 300, .999 * price, 0);
				box->color_ = { 255, 0, 0 }; //red for resistances
				getPlotData()->annotations.push_back(box);

			}
		}

	}


}

EXPORT_ALGORITHM(PatternBreak)