#include <functional>
#include <atomic>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <thread>
#include <assert.h>
#include "BaseAlgorithm.h"
#include "LocalBroker.h"

class BaseAlgorithm::BaseAlgorithmImpl
{
public:
	explicit BaseAlgorithmImpl(BaseAlgorithm* parent, std::string input, std::shared_ptr<InteractiveBrokersClient> ibApiPtr, bool live);
	~BaseAlgorithmImpl();
	std::shared_ptr<PlotData> plotData;
	void run();
	void stop();
	std::string ticker();

// ordering functions
public:
	PositionId longMarketNoStop(std::string ticker, int numShares);
	PositionId longMarketStopMarket(std::string ticker, int numShares, double stopPrice);
	PositionId longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	PositionId shortMarketNoStop(std::string ticker, int numShares);
	PositionId shortMarketStopMarket(std::string ticker, int numShares, double activationPrice);
	PositionId shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice);
	PositionId shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice);
	PositionId shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice);

	void closePosition(PositionId posId);
	Position getPosition(PositionId posId);

private:
	std::string ticker_;

	BaseAlgorithm* parent;
	LocalBroker localBroker;
	bool running;

	CallbackHandle callbackHandle;
	void tickHandler(const Tick& tick);

// algorithm benchmarking
private:
	double profit_;
};

BaseAlgorithm::BaseAlgorithmImpl::BaseAlgorithmImpl(BaseAlgorithm* parentIn, std::string input, std::shared_ptr<InteractiveBrokersClient> ibApiPtr, bool live) :
	parent(parentIn),
	plotData(std::make_shared<PlotData>()),
	localBroker(input, ibApiPtr, live),
	running(false)
{
	auto extensionIndex = input.find(".tickdat");
	if (extensionIndex != std::wstring::npos)
	{
		auto isValidTickerChar = [](char c) 
		{
			return (c <= 'z' && c >= 'a') || (c <= 'Z' && c >= 'A') || c == '.';
		};

		// loop until first invalid character
		for (auto i = static_cast<std::string::size_type>(extensionIndex) - 1; i >= 0 && isValidTickerChar(input[i]); --i)
		{
			ticker_.push_back(input[i]);
		}
		std::reverse(ticker_.begin(), ticker_.end());
	}
	else
	{
		ticker_ = input;
	}

	callbackHandle = localBroker.registerListener([this](const Tick& tick)
	{
		this->tickHandler(tick);
	});

	// we can initialize profit here because tickHandler won't run until 
	// run is called
	profit_ = 0;
}

BaseAlgorithm::BaseAlgorithmImpl::~BaseAlgorithmImpl()
{
	stop();
}

void BaseAlgorithm::BaseAlgorithmImpl::run()
{
	if(!running)
	{
		running = true;
		localBroker.run();
	}
}

void BaseAlgorithm::BaseAlgorithmImpl::stop()
{
	if (running)
	{
		running = false;
		//unregistering the callback stops the algorithm
		localBroker.unregisterListener(callbackHandle);
	}
}

std::string BaseAlgorithm::BaseAlgorithmImpl::ticker()
{
	return ticker_;
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longMarketNoStop(std::string ticker, int numShares)
{
	// when an order gets filled, this lambda submits an annotation
	return localBroker.longMarket(ticker, numShares, [this, ticker, numShares](double avgFillPrice, time_t time)
	{
		std::string labelText = "Long " + std::to_string(numShares) + " shares at $" + std::to_string(avgFillPrice) + "\n";
		std::lock_guard<std::mutex> lock(plotData->plotDataMtx);
		plotData->annotations.push_back(std::make_shared<Annotation::Label>(labelText, time, avgFillPrice));
	});	
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return 0;
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return 0;
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return 0;
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return 0;
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortMarketNoStop(std::string ticker, int numShares)
{
	return localBroker.shortMarket(ticker, numShares, [this, numShares](double avgFillPrice, time_t time)
	{
		std::string labelText = "Short " + std::to_string(numShares) + " shares at $" + std::to_string(avgFillPrice) + "\n";

		std::lock_guard<std::mutex> lock(plotData->plotDataMtx);
		plotData->annotations.push_back(std::make_shared<Annotation::Label>(labelText, time, avgFillPrice));
	});
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return 0;
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return 0;
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return 0;
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return 0;
}

void BaseAlgorithm::BaseAlgorithmImpl::closePosition(PositionId posId)
{
	// when an order gets filled, this lambda submits an annotation
	localBroker.closePosition(posId, [this, posId](double avgFillPrice, time_t time)
	{
		auto position = localBroker.getPosition(posId);
		profit_ += position.profit;
		std::string labelText = "Closing Position at $" + std::to_string(avgFillPrice) + "\n";
		labelText += "Position Profit : " + std::to_string(position.profit) + "\n";
		labelText += "Net Profit : " + std::to_string(profit_) + "\n";

		// shift the closing label down a bit
		auto labelAnnotation = std::make_shared<Annotation::Label>(labelText, time, avgFillPrice);
		auto lineAnnotation = std::make_shared<Annotation::Line>(position.openTime, position.averagePrice, position.closeTime, avgFillPrice);
		if (position.profit < 0)
		{
			labelAnnotation->color_ = { 255, 0, 0 };
			lineAnnotation->color_ = { 255, 0, 0};

		}
		else if (position.profit > 0)
		{
			labelAnnotation->color_ = { 0, 255, 0 };
			lineAnnotation->color_ = { 0, 255, 0 };
		}

		std::lock_guard<std::mutex> lock(plotData->plotDataMtx);
		plotData->annotations.push_back(labelAnnotation);
		plotData->annotations.push_back(lineAnnotation);
	});

}

Position BaseAlgorithm::BaseAlgorithmImpl::getPosition(PositionId posId)
{
	return localBroker.getPosition(posId);
}

void BaseAlgorithm::BaseAlgorithmImpl::tickHandler(const Tick & tick)
{
	parent->tickHandler(tick);

	// in case if gui is sometimes slow and is holding the lock, we don't want to block this thread
	// therefore, if we fail to retrieve the lock immediately, we push the data into a secondary buffer.
	// when we successfully get the lock, we simply have to move the data into plotData. this will allow
	// the algorithm to be more overall responsive .
	std::unique_lock<std::mutex> lock(plotData->plotDataMtx);
	PlotData& plotDataRef = *plotData;
	//this is the only thread that accesses buffer no synchronization needed
	plotData->ticks.push_back(tick);
}

BaseAlgorithm::BaseAlgorithm(std::string input, std::shared_ptr<InteractiveBrokersClient> ibApiPtr, bool live):
	impl_(new BaseAlgorithmImpl(this, input, ibApiPtr, live))
{
}

BaseAlgorithm::~BaseAlgorithm()
{
	if (impl_ != nullptr)
	{
		delete impl_;
		impl_ = nullptr;
	}
}

std::shared_ptr<PlotData> BaseAlgorithm::getPlotData()
{
	return impl_->plotData;
}

void BaseAlgorithm::run()
{
	if (impl_ != nullptr)
	{
		impl_->run();
	}
}

void BaseAlgorithm::stop()
{
	if (impl_ != nullptr)
	{
		impl_->stop();
	}
}

PositionId BaseAlgorithm::longMarketNoStop(std::string ticker, int numShares)
{
	return impl_->longMarketNoStop(ticker, numShares);
}

PositionId BaseAlgorithm::longMarketStopMarket(std::string ticker, int numShares, double stopPrice)
{
	return impl_->longMarketStopMarket(ticker, numShares, stopPrice);
}

PositionId BaseAlgorithm::longMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return impl_->longMarketStopLimit(ticker, numShares, activationPrice, limitPrice);
}

PositionId BaseAlgorithm::longLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return impl_->longLimitStopMarket(ticker, numShares, buyLimit, activationPrice);
}

PositionId BaseAlgorithm::longLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return impl_->longLimitStopLimit(ticker, numShares, buyLimit, activationPrice, limitPrice);
}

PositionId BaseAlgorithm::shortMarketNoStop(std::string ticker, int numShares)
{
	return impl_->shortMarketNoStop(ticker, numShares);
}

PositionId BaseAlgorithm::shortMarketStopMarket(std::string ticker, int numShares, double activationPrice)
{
	return impl_->shortMarketStopMarket(ticker, numShares, activationPrice);
}

PositionId BaseAlgorithm::shortMarketStopLimit(std::string ticker, int numShares, double activationPrice, double limitPrice)
{
	return impl_->shortMarketStopLimit(ticker, numShares, activationPrice, limitPrice);
}

PositionId BaseAlgorithm::shortLimitStopMarket(std::string ticker, int numShares, double buyLimit, double activationPrice)
{
	return impl_->shortLimitStopMarket(ticker, numShares, buyLimit, activationPrice);
}

PositionId BaseAlgorithm::shortLimitStopLimit(std::string ticker, int numShares, double buyLimit, double activationPrice, double limitPrice)
{
	return impl_->shortLimitStopLimit(ticker, numShares, buyLimit, activationPrice, limitPrice);
}

void BaseAlgorithm::closePosition(PositionId posId)
{
	impl_->closePosition(posId);
}

void BaseAlgorithm::reducePosition(PositionId posId, int numShares)
{

}

Position BaseAlgorithm::getPosition(PositionId posId)
{
	return impl_->getPosition(posId);
}

std::string BaseAlgorithm::ticker()
{
	return impl_->ticker();
}

bool BaseAlgorithm::isRth(time_t time)
{
	char timeCStr[256];
	ctime_s(timeCStr, 256, &time);

	std::string timeStr(timeCStr);

	// we need to convert it to local time in order to account
	// for DLS. cannot use simple modulus arithmetic to get
	// RTH. we can simply replace all the colons with 0s
	// and compare the raw values as integers
	timeStr[13] = '0';
	timeStr[16] = '0';
	auto timeInt = std::stoi(timeStr.substr(11, 8));
	if (timeInt >= 8030000 && timeInt < 15000000)
	{
		return true;
	}

	return false;
}
