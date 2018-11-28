#include "LocalBroker.h"

LocalBroker::LocalBroker(std::string input, std::shared_ptr<InteractiveBrokersClient> ibApi, bool live) :
	ibApi_(ibApi),
	tickSource_(input, ibApi),
	liveTrade_(live)
{
	// if live option is turned on but invalid conection is provided
	// then we can't trade live.
	// when trading live, all orders are sent to ib and all positions are
	// maintained by IB. 
	if (live)
	{
		if (ibApi == nullptr || !ibApi->isReady())
		{
			throw std::runtime_error("A valid IB connection is needed for live trading.");
		}
	}
	// for non live orders, we maintain all positions and orders locally.
	// stoplosses are handled by stoplossHandler
	else
	{
		//register stophandler for tick callback from data source.
		stoplossHandlerHandle = tickSource_.registerListener([this](const Tick& tick)
		{
			this->stoplossHandler(tick);
		});
	}

	uniquePendingOrderIds_ = 0;
}

LocalBroker::~LocalBroker()
{
	tickSource_.unregisterCallback(stoplossHandlerHandle);
}

void LocalBroker::run()
{
	tickSource_.run();
}

bool LocalBroker::valid()
{
	return valid_;
}

PositionId LocalBroker::longMarketNoStop(std::string ticker, int numShares, std::function<void(double avgPrice)> callerFillNotification)
{
	//validate number of shares
	numShares = abs(numShares);
	// for market long orders, we don't need to specify limit prices
	pendingOrders_[uniquePendingOrderIds_] = std::make_unique<SimpleOrder>(SimpleOrder{ SimpleOrder::OrderType::LONGMARKETNOSTOP, 0, 0, numShares });

	// increment pending order ids
	return uniquePendingOrderIds_++;
}

PositionId LocalBroker::longMarketStopMarket(std::string ticker, int numShares, double stopPrice, std::function<void(double avgPrice)> callerFillNotification)
{
	return PositionId();
}

PositionId LocalBroker::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice, std::function<void(double avgPrice)> callerFillNotification)
{
	return PositionId();
}

PositionId LocalBroker::longLimitStopMarket(std::string ticker, int numShares, double longLimit, double activationPrice, std::function<void(double avgPrice)> callerFillNotification)
{
	return PositionId();
}

PositionId LocalBroker::longLimitStopLimit(std::string ticker, int numShares, double longLimit, double activationPrice, double limitPrice, std::function<void(double avgPrice)> callerFillNotification)
{
	return PositionId();
}

PositionId LocalBroker::shortMarketNoStop(std::string ticker, int numShares, std::function<void(double avgPrice)> callerFillNotification)
{
	//validate number of shares
	numShares = abs(numShares);
	// for market long orders, we don't need to specify limit prices
	pendingOrders_[uniquePendingOrderIds_] = std::make_unique<SimpleOrder>(SimpleOrder{ SimpleOrder::OrderType::SHORTMARKETNOSTOP, 0, 0, -numShares });

	// return the pending order id and increment
	return uniquePendingOrderIds_++;
}

PositionId LocalBroker::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice, std::function<void(double avgPrice)> callerFillNotification)
{
	return PositionId();
}

PositionId LocalBroker::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice, std::function<void(double avgPrice)> callerFillNotification)
{
	return PositionId();
}

PositionId LocalBroker::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice, std::function<void(double avgPrice)> callerFillNotification)
{
	return PositionId();
}

PositionId LocalBroker::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice, std::function<void(double avgPrice)> callerFillNotification)
{
	return PositionId();
}

void LocalBroker::reducePosition(PositionId posId, int numShares)
{
	if (liveTrade_)
	{
		// we are supposed to create an order and submit it to ibapi. ib api
		// returns an orderId to us. We need to map this OrderId to to a position 
		// that the orderUpdate callback from ibapi will be able to update the appropriate
		// position

		// all orders are submitted as all or none

		// skip this part for now. we don't need it until we find a working algo

	}
	//if not live trading, then all market positions are filled instantly at the last seen price
	else
	{
		// autofill
		portfolio_.reducePosition(posId, tickSource_.lastPrice(), numShares);
	}
}

Position LocalBroker::getPosition(PositionId posId)
{
	return Position();
}

void LocalBroker::executePosition(PositionId posId, std::function<void(double avgPrice)> callerFillNotification)
{

	// validate posId by checking for a pending order
	if (pendingOrders_.find(posId) != pendingOrders_.end())
	{
		const auto& pendingOrder = pendingOrders_[posId];			
		
		// this lambda function gets called when the position is filled
		auto fillNotification = [this, posId, callerFillNotification](double avgFillPrice)
		{
			portfolio_.fillPosition(posId, avgFillPrice, pendingOrders_[posId]->shares);
			callerFillNotification(posId);
		};

		if (liveTrade_)
		{
			// if live trading then create an ib order and submit it. 
			// register fillNotification as the callback when ib fills
			// the order
			switch (pendingOrder->type)
			{
			case SimpleOrder::OrderType::LONGMARKETNOSTOP:
			{

			}
			break;
			case SimpleOrder::OrderType::LONGMARKETSTOPMARKET:
			{

			}
			break;
			case SimpleOrder::OrderType::LONGMARKETSTOPLIMIT:
			{

			}
			break;
			case SimpleOrder::OrderType::LONGLIMITSTOPMARKET:
			{

			}
			break;
			case SimpleOrder::OrderType::LONGLIMITSTOPLIMIT:
			{

			}
			break;
			case SimpleOrder::OrderType::SHORTMARKETNOSTOP:
			{

			}
			break;
			case SimpleOrder::OrderType::SHORTMARKETSTOPMARKET:
			{

			}
			break;
			case SimpleOrder::OrderType::SHORTMARKETSTOPLIMIT:
			{

			}
			break;
			case SimpleOrder::OrderType::SHORTLIMITSTOPMARKET:
			{

			}
			break;
			case SimpleOrder::OrderType::SHORTLIMITSTOPLIMIT:
			{

			}
			break;
			}

		}
		else
		{
			// for non live trades, we simply fill the position with the latest price
			// instantly
			fillNotification(tickSource_.lastPrice());
		}

		// no longer pending after execution
		pendingOrders_.erase(posId);
	}
}

CallbackHandle LocalBroker::registerListener(TickListener callback)
{
	return tickSource_.registerListener(callback);
}

void LocalBroker::unregisterListener(CallbackHandle handle)
{
	tickSource_.unregisterCallback(handle);
}

void LocalBroker::stoplossHandler(const Tick & tick)
{
}

void LocalBroker::closePosition(PositionId posId)
{
	if (liveTrade_)
	{
		// we are supposed to create an order and submit it to ibapi. ib api
		// returns an orderId to us. We need to map this OrderId to to a position 
		// that the orderUpdate callback from ibapi will be able to update the appropriate
		// position

		// all orders are submitted as all or none

		// skip this part for now. we don't need it until we find a working algo

	}
	//if not live trading, then all market positions are filled instantly at the last seen price
	else
	{
		// autofill
		portfolio_.closePosition(posId, tickSource_.lastPrice());
	}
}
