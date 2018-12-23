// PatternBreak.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "../../BaseModules/BaseAlgorithm/BaseAlgorithm.h"
#include "../../BaseModules/CandleMaker/CandleMaker.h"
#include "../../BaseModules/Indicators/SimpleMovingAverage.h"
#include "../../BaseModules/Indicators/ExponentialMovingAverage.h"
#include "../../BaseModules/Indicators/BollingerBands.h"

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

	void tickHandler(const Tick& tick);
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

				auto line = std::make_shared<Annotation::Line>(prevCandle.time, prevdPriceSma, currCandle.time, currdPriceSma, 2);
				line->color_ = { 255, 0, 0 }; //red

				getPlotData()->annotations.push_back(line);
				// zero crossings
				if (prevdPriceSma < 0 && currdPriceSma > 0)
				{
					auto box = std::make_shared<Annotation::Box>(currCandle.time - 300, 1.001 * currCandle.close, currCandle.time + 300, .999 * currCandle.close , 0);
					box->color_ = { 0, 255, 0 }; //green

					getPlotData()->annotations.push_back(box);
				}
				else if (prevdPriceSma > 0 && currdPriceSma < 0)
				{

					auto box = std::make_shared<Annotation::Box>(currCandle.time - 300, 1.001 * currCandle.close, currCandle.time + 300, .999 * currCandle.close, 0);
					box->color_ = { 0, 0, 255 }; //blue

					getPlotData()->annotations.push_back(box);
				}
			}

		}
		
	}
}

EXPORT_ALGORITHM(PatternBreak)