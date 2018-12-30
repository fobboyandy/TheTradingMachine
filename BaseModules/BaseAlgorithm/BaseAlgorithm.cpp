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
	PositionId longMarket(std::string ticker, int numShares);
	PositionId longLimit(std::string ticker, double limitPrice, int numShares);

	PositionId shortMarket(std::string ticker, int numShares);
	PositionId shortLimit(std::string ticker, double limitPrice, int numShares);

	// Each time we reduce a position, remember the previous position so that we can
	// track changes relative to the previous position
	Position pPosition;
	void closePosition(PositionId posId);
	void reducePosition(PositionId posId, int numShares);
	Position getPosition(PositionId posId);

private:
	std::string ticker_;

	BaseAlgorithm* parent;
	LocalBroker localBroker;
	bool running;

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

	localBroker.registerListener([this](const Tick& tick)
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
	}
}

std::string BaseAlgorithm::BaseAlgorithmImpl::ticker()
{
	return ticker_;
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longMarket(std::string ticker, int numShares)
{
	// when an order gets filled, this lambda submits an annotation
	return localBroker.longMarket(ticker, numShares, [this, ticker, numShares](double avgFillPrice, time_t time)
	{
		std::string labelText = "Long " + std::to_string(numShares) + " shares at $" + std::to_string(avgFillPrice) + "\n";
		// initialize a new pPosition
		pPosition.openTime = time;
		pPosition.averagePrice = avgFillPrice;
		pPosition.shares = numShares;
		pPosition.profit = 0;

		std::lock_guard<std::mutex> lock(plotData->plotDataMtx);
		plotData->annotations.push_back(std::make_shared<Annotation::Label>(labelText, time, avgFillPrice));
	});	
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::longLimit(std::string ticker, double limitPrice, int numShares)
{
	return PositionId();
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortMarket(std::string ticker, int numShares)
{
	return localBroker.shortMarket(ticker, numShares, [this, numShares](double avgFillPrice, time_t time)
	{
		std::string labelText = "Short " + std::to_string(numShares) + " shares at $" + std::to_string(avgFillPrice) + "\n";
		// initialize a new pPosition
		pPosition.openTime = time;
		pPosition.averagePrice = avgFillPrice;
		pPosition.shares = -abs(numShares);
		pPosition.profit = 0;
		std::lock_guard<std::mutex> lock(plotData->plotDataMtx);
		plotData->annotations.push_back(std::make_shared<Annotation::Label>(labelText, time, avgFillPrice));
	});
}

PositionId BaseAlgorithm::BaseAlgorithmImpl::shortLimit(std::string ticker, double limitPrice, int numShares)
{
	return PositionId();
}


void BaseAlgorithm::BaseAlgorithmImpl::closePosition(PositionId posId)
{
	// when an order gets filled, this lambda submits an annotation
	localBroker.closePosition(posId, [this, posId](double avgFillPrice, time_t time)
	{
		auto position = localBroker.getPosition(posId);
		profit_ += position.profit;
		std::string labelText = "Closing Position at $" + std::to_string(avgFillPrice) + "\n";
		labelText += "Profit Change: " + std::to_string(position.profit - pPosition.profit) + "\n";
		labelText += "Net Profit : " + std::to_string(profit_) + "\n";

		auto labelAnnotation = std::make_shared<Annotation::Label>(labelText, time, avgFillPrice);
		auto lineAnnotation = std::make_shared<Annotation::Line>(pPosition.openTime, pPosition.averagePrice, position.closeTime, avgFillPrice);
		if (position.profit - pPosition.profit < 0)
		{
			labelAnnotation->color_ = { 255, 0, 0 };
			lineAnnotation->color_ = { 255, 0, 0};

		}
		else if (position.profit - pPosition.profit > 0)
		{
			labelAnnotation->color_ = { 0, 255, 0 };
			lineAnnotation->color_ = { 0, 255, 0 };
		}

		std::lock_guard<std::mutex> lock(plotData->plotDataMtx);
		plotData->annotations.push_back(labelAnnotation);
		plotData->annotations.push_back(lineAnnotation);
	});

}

void BaseAlgorithm::BaseAlgorithmImpl::reducePosition(PositionId posId, int numShares)
{
	if (posId != -1)
	{
		// when an order gets filled, this lambda submits an annotation
		localBroker.reducePosition(posId, numShares, [this, posId](double avgFillPrice, time_t time)
		{
			auto cPosition = localBroker.getPosition(posId);
			std::string labelText = "Reducing Position at $" + std::to_string(avgFillPrice) + "\n";

			// for price reductions, we print the current status but don't add to the net profit
			labelText += "Profit Change : " + std::to_string(cPosition.profit - pPosition.profit) + "\n";
			labelText += "Net Profit : " + std::to_string(profit_ + cPosition.profit) + "\n";

			// shift the closing label down a bit
			auto labelAnnotation = std::make_shared<Annotation::Label>(labelText, time, avgFillPrice);
			auto lineAnnotation = std::make_shared<Annotation::Line>(pPosition.openTime, pPosition.averagePrice, time, avgFillPrice);
			if (cPosition.profit - pPosition.profit < 0)
			{
				labelAnnotation->color_ = { 255, 0, 0 };
				lineAnnotation->color_ = { 255, 0, 0 };

			}
			else if (cPosition.profit - pPosition.profit > 0)
			{
				labelAnnotation->color_ = { 0, 255, 0 };
				lineAnnotation->color_ = { 0, 255, 0 };
			}

			//update pPosition
			pPosition.openTime = time;
			pPosition.averagePrice = avgFillPrice;
			pPosition.profit = cPosition.profit;

			std::lock_guard<std::mutex> lock(plotData->plotDataMtx);
			plotData->annotations.push_back(labelAnnotation);
			plotData->annotations.push_back(lineAnnotation);
		});
	}
	
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

PositionId BaseAlgorithm::longMarket(std::string ticker, int numShares)
{
	return impl_->longMarket(ticker, numShares);
}

PositionId BaseAlgorithm::longLimit(std::string ticker, double limitPrice, int numShares)
{
	return impl_->longLimit(ticker, limitPrice, numShares);
}

PositionId BaseAlgorithm::shortMarket(std::string ticker, int numShares)
{
	return impl_->shortMarket(ticker, numShares);
}

PositionId BaseAlgorithm::shortLimit(std::string ticker, double limitPrice, int numShares)
{
	return impl_->shortLimit(ticker, limitPrice, numShares);
}

void BaseAlgorithm::closePosition(PositionId posId)
{
	impl_->closePosition(posId);
}

void BaseAlgorithm::reducePosition(PositionId posId, int numShares)
{
	impl_->reducePosition(posId, numShares);
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
