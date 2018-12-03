#include "SupportResistanceFinder.h"
#include <memory>
#include <algorithm>

SupportResistanceFinder::SupportResistanceFinder(ALGORITHM_ARGS):
	BaseAlgorithm(BASEALGORITHM_PASS_ARGS),
	candleMaker_(60),
	candlesEma_(5),
	emaEma_(5)
{

}

SupportResistanceFinder::~SupportResistanceFinder()
{
}

void SupportResistanceFinder::tickHandler(const Tick & tick)
{
	Bar candle;
	time_t candleTime;
	bool isNewCandle;
	// create sma for each new candle
	if (candleMaker_.updateCandle(tick, candle, candleTime, isNewCandle) && isNewCandle)
	{
		candles.push_back(pCandle);

		candleEmaPoints.push_back(candlesEma_.computeIndicatorPoint(pCandle.close)[ExponentialMovingAverage::EXPONENTIAL_MOVING_AVERAGE]);
		emaEmaPoints.push_back(emaEma_.computeIndicatorPoint(candleEmaPoints.back())[ExponentialMovingAverage::EXPONENTIAL_MOVING_AVERAGE]);

		if (candleEmaPoints.size() >= 2)
		{
			auto candleEmaLine = std::make_shared<Annotation::Line>(pCandleTime, candleEmaPoints[candleEmaPoints.size() - 2], candleTime, candleEmaPoints.back());
			candleEmaLine->setColor(255, 0, 0); //red

			auto emaEmaLine = std::make_shared<Annotation::Line>(pCandleTime, emaEmaPoints[emaEmaPoints.size() - 2], candleTime, emaEmaPoints.back());
			emaEmaLine->setColor(0, 255, 0); //green

			getPlotData()->annotations.push_back(candleEmaLine);
			getPlotData()->annotations.push_back(emaEmaLine);
		}

		pCandleTime = candleTime;

		// how many consecutive candles above the ema are we looking for
		if (candleEmaPoints.size() >= 2)
		{

			auto p2Candle = candles[candles.size() - 2];
			auto p1Candle = candles[candles.size() - 1];

			auto p2CandleEma = candleEmaPoints[candleEmaPoints.size() - 2];
			auto p1CandleEma = candleEmaPoints[candleEmaPoints.size() - 1];

			// if prev 2 candles don't touch the ema
			if (!isWithin(p2CandleEma, p2Candle.low, p2Candle.high) && !isWithin(p1CandleEma, p1Candle.low, p1Candle.high))
			{

				// if the ema is not within the candle, then simply check whether it is above or below
				// the ema by testing any point with the ema
				if (p2Candle.close > p2CandleEma && p1Candle.close > p1CandleEma)
				{
					if (posType == -1 || posType == 1)
					{
						posType = 0;

						// close the existing position
						if (posId != -1)
						{
							closePosition(posId);
						}
						posId = longMarketNoStop(ticker(), 100);
					}

				}
				else if(p2Candle.close < p2CandleEma && p1Candle.close < p1CandleEma)
				{

					if (posType == -1 || posType == 0)
					{
						posType = 1;
						// close the existing position and start a short
						if (posId != -1)
						{
							closePosition(posId);
						}
						posId = shortMarketNoStop(ticker(), 100);
					}
				}
			}
		}
	}
	pCandle = candle;
}

EXPORT_ALGORITHM(SupportResistanceFinder)
