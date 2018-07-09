/* Copyright (C) 2018 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
* and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#include "StdAfx.h"

#include "IBInterface.h"

#include "EClientSocket.h"
#include "EPosixClientSocketPlatform.h"

#include "Contract.h"
#include "Order.h"
#include "OrderState.h"
#include "Execution.h"
#include "CommissionReport.h"
#include "ContractSamples.h"
#include "OrderSamples.h"
#include "ScannerSubscription.h"
#include "ScannerSubscriptionSamples.h"
#include "executioncondition.h"
#include "PriceCondition.h"
#include "MarginCondition.h"
#include "PercentChangeCondition.h"
#include "TimeCondition.h"
#include "VolumeCondition.h"
#include "AvailableAlgoParams.h"
#include "FAMethodSamples.h"
#include "CommonDefs.h"
#include "AccountSummaryTags.h"
#include "Utils.h"

#include <stdio.h>
#include <chrono>
#include <iostream>
#include <thread>
#include <ctime>
#include <fstream>
#include <cstdint>

const int PING_DEADLINE = 2; // seconds
const int SLEEP_BETWEEN_PINGS = 30; // seconds

bool IBInterface::Initialize()
{
	int attempts = 0;
	while (!isConnected() && attempts < MAX_ATTEMPTS)
	{
		connect("", 7496, 0);
		attempts++;
	}

	if (attempts == MAX_ATTEMPTS)
		return false;

	//wait and process all the initial messages from TWS
	//TWS will be ready when TWS returns nextValidID
	while (!ready)
	{
		Sleep(500);
		processMessages();
	}

	return true;
}

///////////////////////////////////////////////////////////
									// member funcs
									//! [socket_init]
IBInterface::IBInterface() :
	m_osSignal(2000)//2-seconds timeout
	, m_pClient(new EClientSocket(this, &m_osSignal))
	, m_sleepDeadline(0)
	, m_orderId(0)
	, m_pReader(0)
	, m_extraAuth(false)
	, ready(false)
{
}
//! [socket_init]
IBInterface::~IBInterface()
{
	if (m_pReader)
		delete m_pReader;

	delete m_pClient;
}

bool IBInterface::connect(const char *host, unsigned int port, int clientId)
{
	// trying to connect
	printf("Connecting to %s:%d clientId:%d\n", !(host && *host) ? "127.0.0.1" : host, port, clientId);

	//! [connect]
	bool bRes = m_pClient->eConnect(host, port, clientId, m_extraAuth);
	//! [connect]

	if (bRes) {
		printf("Connected to %s:%d clientId:%d\n", m_pClient->host().c_str(), m_pClient->port(), clientId);
		//! [ereader]
		m_pReader = new EReader(m_pClient, &m_osSignal);
		m_pReader->start();
		//! [ereader]
	}
	else
		printf("Cannot connect to %s:%d clientId:%d\n", m_pClient->host().c_str(), m_pClient->port(), clientId);

	return bRes;
}

void IBInterface::disconnect() const
{
	m_pClient->eDisconnect();

	printf("Disconnected\n");
}

bool IBInterface::isConnected() const
{
	return m_pClient->isConnected();
}

void IBInterface::setConnectOptions(const std::string& connectOptions)
{
	m_pClient->setConnectOptions(connectOptions);
}

void IBInterface::processMessages()
{
	time_t now = time(NULL);

	m_osSignal.waitForSignal();
	errno = 0;
	m_pReader->processMsgs();
}

//////////////////////////////////////////////////////////////////
// methods
//! [connectack]
void IBInterface::connectAck() {
	if (!m_extraAuth && m_pClient->asyncEConnect())
		m_pClient->startApi();
}
//! [connectack]

//! [nextvalidid]
void IBInterface::nextValidId(OrderId orderId)
{
	printf("Next Valid Id: %ld\n", orderId);
	m_orderId = orderId;
	ready = true;
}


void IBInterface::currentTime(long time)
{
	//if (m_state == ST_PING_ACK) {
	//	time_t t = (time_t)time;
	//	struct tm * timeinfo = localtime(&t);
	//	printf("The current date/time is: %s", asctime(timeinfo));

	//	time_t now = ::time(NULL);
	//	m_sleepDeadline = now + SLEEP_BETWEEN_PINGS;

	//	m_state = ST_PING_ACK;
	//}
}

//! [error]
void IBInterface::error(int id, int errorCode, const std::string& errorString)
{
	printf("Error. Id: %d, Code: %d, Msg: %s\n", id, errorCode, errorString.c_str());
}
//! [error]

//! [tickprice]
void IBInterface::tickPrice(TickerId tickerId, TickType field, double price, const TickAttrib& attribs) {
	printf("Tick Price. Ticker Id: %ld, Field: %d, Price: %g, CanAutoExecute: %d, PastLimit: %d, PreOpen: %d\n", tickerId, (int)field, price, attribs.canAutoExecute, attribs.pastLimit, attribs.preOpen);
}
//! [tickprice]

//! [ticksize]
void IBInterface::tickSize(TickerId tickerId, TickType field, int size) {
	printf("Tick Size. Ticker Id: %ld, Field: %d, Size: %d\n", tickerId, (int)field, size);
}
//! [ticksize]

//! [tickoptioncomputation]
void IBInterface::tickOptionComputation(TickerId tickerId, TickType tickType, double impliedVol, double delta,
	double optPrice, double pvDividend,
	double gamma, double vega, double theta, double undPrice) {
	printf("TickOptionComputation. Ticker Id: %ld, Type: %d, ImpliedVolatility: %g, Delta: %g, OptionPrice: %g, pvDividend: %g, Gamma: %g, Vega: %g, Theta: %g, Underlying Price: %g\n", tickerId, (int)tickType, impliedVol, delta, optPrice, pvDividend, gamma, vega, theta, undPrice);
}
//! [tickoptioncomputation]

//! [tickgeneric]
void IBInterface::tickGeneric(TickerId tickerId, TickType tickType, double value) {
	printf("Tick Generic. Ticker Id: %ld, Type: %d, Value: %g\n", tickerId, (int)tickType, value);
}
//! [tickgeneric]

//! [tickstring]
void IBInterface::tickString(TickerId tickerId, TickType tickType, const std::string& value) {
	printf("Tick String. Ticker Id: %ld, Type: %d, Value: %s\n", tickerId, (int)tickType, value.c_str());
}
//! [tickstring]

void IBInterface::tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
	double totalDividends, int holdDays, const std::string& futureLastTradeDate, double dividendImpact, double dividendsToLastTradeDate) {
	printf("TickEFP. %ld, Type: %d, BasisPoints: %g, FormattedBasisPoints: %s, Total Dividends: %g, HoldDays: %d, Future Last Trade Date: %s, Dividend Impact: %g, Dividends To Last Trade Date: %g\n", tickerId, (int)tickType, basisPoints, formattedBasisPoints.c_str(), totalDividends, holdDays, futureLastTradeDate.c_str(), dividendImpact, dividendsToLastTradeDate);
}

//! [orderstatus]
void IBInterface::orderStatus(OrderId orderId, const std::string& status, double filled,
	double remaining, double avgFillPrice, int permId, int parentId,
	double lastFillPrice, int clientId, const std::string& whyHeld, double mktCapPrice) {
	printf("OrderStatus. Id: %ld, Status: %s, Filled: %g, Remaining: %g, AvgFillPrice: %g, PermId: %d, LastFillPrice: %g, ClientId: %d, WhyHeld: %s, MktCapPrice: %g\n", orderId, status.c_str(), filled, remaining, avgFillPrice, permId, lastFillPrice, clientId, whyHeld.c_str(), mktCapPrice);
}
//! [orderstatus]

//! [openorder]
void IBInterface::openOrder(OrderId orderId, const Contract& contract, const Order& order, const OrderState& ostate) {
	//printf("OpenOrder. ID: %ld, %s, %s @ %s: %s, %s, %g, %g, %s, %s\n", orderId, contract.symbol.c_str(), contract.secType.c_str(), contract.exchange.c_str(), order.action.c_str(), order.orderType.c_str(), order.totalQuantity, order.cashQty == UNSET_DOUBLE ? 0 : order.cashQty, ostate.status.c_str(), order.dontUseAutoPriceForHedge ? "true" : "false");
	//if (order.whatIf) {
	//	printf("What-If. ID: %ld, InitMarginBefore: %s, MaintMarginBefore: %s, EquityWithLoanBefore: %s, InitMarginChange: %s, MaintMarginChange: %s, EquityWithLoanChange: %s, InitMarginAfter: %s, MaintMarginAfter: %s, EquityWithLoanAfter: %s\n",
	//		orderId, Utils::formatDoubleString(ostate.initMarginBefore).c_str(), Utils::formatDoubleString(ostate.maintMarginBefore).c_str(), Utils::formatDoubleString(ostate.equityWithLoanBefore).c_str(),
	//		Utils::formatDoubleString(ostate.initMarginChange).c_str(), Utils::formatDoubleString(ostate.maintMarginChange).c_str(), Utils::formatDoubleString(ostate.equityWithLoanChange).c_str(),
	//		Utils::formatDoubleString(ostate.initMarginAfter).c_str(), Utils::formatDoubleString(ostate.maintMarginAfter).c_str(), Utils::formatDoubleString(ostate.equityWithLoanAfter).c_str());
	//}
}
//! [openorder]

//! [openorderend]
void IBInterface::openOrderEnd() {
	printf("OpenOrderEnd\n");
}
//! [openorderend]

void IBInterface::winError(const std::string& str, int lastError) {}
void IBInterface::connectionClosed() {
	printf("Connection Closed\n");
}

//! [updateaccountvalue]
void IBInterface::updateAccountValue(const std::string& key, const std::string& val,
	const std::string& currency, const std::string& accountName) {
	printf("UpdateAccountValue. Key: %s, Value: %s, Currency: %s, Account Name: %s\n", key.c_str(), val.c_str(), currency.c_str(), accountName.c_str());
}
//! [updateaccountvalue]

//! [updateportfolio]
void IBInterface::updatePortfolio(const Contract& contract, double position,
	double marketPrice, double marketValue, double averageCost,
	double unrealizedPNL, double realizedPNL, const std::string& accountName) {
	printf("UpdatePortfolio. %s, %s @ %s: Position: %g, MarketPrice: %g, MarketValue: %g, AverageCost: %g, UnrealizedPNL: %g, RealizedPNL: %g, AccountName: %s\n", (contract.symbol).c_str(), (contract.secType).c_str(), (contract.primaryExchange).c_str(), position, marketPrice, marketValue, averageCost, unrealizedPNL, realizedPNL, accountName.c_str());
}
//! [updateportfolio]

//! [updateaccounttime]
void IBInterface::updateAccountTime(const std::string& timeStamp) {
	printf("UpdateAccountTime. Time: %s\n", timeStamp.c_str());
}
//! [updateaccounttime]

//! [accountdownloadend]
void IBInterface::accountDownloadEnd(const std::string& accountName) {
	printf("Account download finished: %s\n", accountName.c_str());
}
//! [accountdownloadend]

//! [contractdetails]
void IBInterface::contractDetails(int reqId, const ContractDetails& contractDetails) {
	printf("ContractDetails begin. ReqId: %d\n", reqId);
	printContractMsg(contractDetails.contract);
	printContractDetailsMsg(contractDetails);
	printf("ContractDetails end. ReqId: %d\n", reqId);
}
//! [contractdetails]

//! [bondcontractdetails]
void IBInterface::bondContractDetails(int reqId, const ContractDetails& contractDetails) {
	printf("BondContractDetails begin. ReqId: %d\n", reqId);
	printBondContractDetailsMsg(contractDetails);
	printf("BondContractDetails end. ReqId: %d\n", reqId);
}
//! [bondcontractdetails]

void IBInterface::printContractMsg(const Contract& contract) {
	printf("\tConId: %ld\n", contract.conId);
	printf("\tSymbol: %s\n", contract.symbol.c_str());
	printf("\tSecType: %s\n", contract.secType.c_str());
	printf("\tLastTradeDateOrContractMonth: %s\n", contract.lastTradeDateOrContractMonth.c_str());
	printf("\tStrike: %g\n", contract.strike);
	printf("\tRight: %s\n", contract.right.c_str());
	printf("\tMultiplier: %s\n", contract.multiplier.c_str());
	printf("\tExchange: %s\n", contract.exchange.c_str());
	printf("\tPrimaryExchange: %s\n", contract.primaryExchange.c_str());
	printf("\tCurrency: %s\n", contract.currency.c_str());
	printf("\tLocalSymbol: %s\n", contract.localSymbol.c_str());
	printf("\tTradingClass: %s\n", contract.tradingClass.c_str());
}

void IBInterface::printContractDetailsMsg(const ContractDetails& contractDetails) {
	printf("\tMarketName: %s\n", contractDetails.marketName.c_str());
	printf("\tMinTick: %g\n", contractDetails.minTick);
	printf("\tPriceMagnifier: %ld\n", contractDetails.priceMagnifier);
	printf("\tOrderTypes: %s\n", contractDetails.orderTypes.c_str());
	printf("\tValidExchanges: %s\n", contractDetails.validExchanges.c_str());
	printf("\tUnderConId: %d\n", contractDetails.underConId);
	printf("\tLongName: %s\n", contractDetails.longName.c_str());
	printf("\tContractMonth: %s\n", contractDetails.contractMonth.c_str());
	printf("\tIndystry: %s\n", contractDetails.industry.c_str());
	printf("\tCategory: %s\n", contractDetails.category.c_str());
	printf("\tSubCategory: %s\n", contractDetails.subcategory.c_str());
	printf("\tTimeZoneId: %s\n", contractDetails.timeZoneId.c_str());
	printf("\tTradingHours: %s\n", contractDetails.tradingHours.c_str());
	printf("\tLiquidHours: %s\n", contractDetails.liquidHours.c_str());
	printf("\tEvRule: %s\n", contractDetails.evRule.c_str());
	printf("\tEvMultiplier: %g\n", contractDetails.evMultiplier);
	printf("\tMdSizeMultiplier: %d\n", contractDetails.mdSizeMultiplier);
	printf("\tAggGroup: %d\n", contractDetails.aggGroup);
	printf("\tUnderSymbol: %s\n", contractDetails.underSymbol.c_str());
	printf("\tUnderSecType: %s\n", contractDetails.underSecType.c_str());
	printf("\tMarketRuleIds: %s\n", contractDetails.marketRuleIds.c_str());
	printf("\tRealExpirationDate: %s\n", contractDetails.realExpirationDate.c_str());
	printf("\tLastTradeTime: %s\n", contractDetails.lastTradeTime.c_str());
	printContractDetailsSecIdList(contractDetails.secIdList);
}

void IBInterface::printContractDetailsSecIdList(const TagValueListSPtr &secIdList) {
	const int secIdListCount = secIdList.get() ? secIdList->size() : 0;
	if (secIdListCount > 0) {
		printf("\tSecIdList: {");
		for (int i = 0; i < secIdListCount; ++i) {
			const TagValue* tagValue = ((*secIdList)[i]).get();
			printf("%s=%s;", tagValue->tag.c_str(), tagValue->value.c_str());
		}
		printf("}\n");
	}
}

void IBInterface::printBondContractDetailsMsg(const ContractDetails& contractDetails) {
	printf("\tSymbol: %s\n", contractDetails.contract.symbol.c_str());
	printf("\tSecType: %s\n", contractDetails.contract.secType.c_str());
	printf("\tCusip: %s\n", contractDetails.cusip.c_str());
	printf("\tCoupon: %g\n", contractDetails.coupon);
	printf("\tMaturity: %s\n", contractDetails.maturity.c_str());
	printf("\tIssueDate: %s\n", contractDetails.issueDate.c_str());
	printf("\tRatings: %s\n", contractDetails.ratings.c_str());
	printf("\tBondType: %s\n", contractDetails.bondType.c_str());
	printf("\tCouponType: %s\n", contractDetails.couponType.c_str());
	printf("\tConvertible: %s\n", contractDetails.convertible ? "yes" : "no");
	printf("\tCallable: %s\n", contractDetails.callable ? "yes" : "no");
	printf("\tPutable: %s\n", contractDetails.putable ? "yes" : "no");
	printf("\tDescAppend: %s\n", contractDetails.descAppend.c_str());
	printf("\tExchange: %s\n", contractDetails.contract.exchange.c_str());
	printf("\tCurrency: %s\n", contractDetails.contract.currency.c_str());
	printf("\tMarketName: %s\n", contractDetails.marketName.c_str());
	printf("\tTradingClass: %s\n", contractDetails.contract.tradingClass.c_str());
	printf("\tConId: %ld\n", contractDetails.contract.conId);
	printf("\tMinTick: %g\n", contractDetails.minTick);
	printf("\tMdSizeMultiplier: %d\n", contractDetails.mdSizeMultiplier);
	printf("\tOrderTypes: %s\n", contractDetails.orderTypes.c_str());
	printf("\tValidExchanges: %s\n", contractDetails.validExchanges.c_str());
	printf("\tNextOptionDate: %s\n", contractDetails.nextOptionDate.c_str());
	printf("\tNextOptionType: %s\n", contractDetails.nextOptionType.c_str());
	printf("\tNextOptionPartial: %s\n", contractDetails.nextOptionPartial ? "yes" : "no");
	printf("\tNotes: %s\n", contractDetails.notes.c_str());
	printf("\tLong Name: %s\n", contractDetails.longName.c_str());
	printf("\tEvRule: %s\n", contractDetails.evRule.c_str());
	printf("\tEvMultiplier: %g\n", contractDetails.evMultiplier);
	printf("\tAggGroup: %d\n", contractDetails.aggGroup);
	printf("\tMarketRuleIds: %s\n", contractDetails.marketRuleIds.c_str());
	printf("\tTimeZoneId: %s\n", contractDetails.timeZoneId.c_str());
	printf("\tLastTradeTime: %s\n", contractDetails.lastTradeTime.c_str());
	printContractDetailsSecIdList(contractDetails.secIdList);
}

void IBInterface::requestRealTimeMinuteBars(string ticker, int timeFrameMinutes, function<void(const Bar&)> callback)
{
	OrderId oid;
	string minString = " min";
	if (timeFrameMinutes > 1)
		minString.push_back('s');

	auto& timeframeOrderMap = stockRealTimeBarOrderIds[ticker];
	if(timeframeOrderMap.find(timeFrameMinutes) != timeframeOrderMap.end())
	{
		oid = timeframeOrderMap[timeFrameMinutes];
	}
	else
	{
		oid = m_orderId;
		m_orderId++;
		timeframeOrderMap[timeFrameMinutes] = oid;

		//
		// Submit the request with historical data with the update option 
		// on
		//
		m_pClient->reqHistoricalData(
			oid,
			createUsStockContract(ticker),
			"", 
			"1 D",
			to_string(timeFrameMinutes) + minString,
			"TRADES", 
			true, 
			1, 
			true, 
			TagValueListSPtr());
		
		stockRealTimeBarCallbacks[oid].timeFrame = timeFrameMinutes;
	}
	stockRealTimeBarCallbacks[oid].callbackFunctions.push_back(callback);

}

void IBInterface::requestHistoricalMinuteBars(string ticker, int timeFrameMinutes, function<void(const Bar&)> callback)
{
	string minString = " min";
	if (timeFrameMinutes > 1)
		minString.push_back('s');
	OrderId oid = m_orderId;
	m_orderId++;
	m_pClient->reqHistoricalData(
		oid,
		createUsStockContract(ticker),
		"",
		"1 D",
		to_string(timeFrameMinutes) + minString,
		"TRADES",
		true,
		1,
		false,
		TagValueListSPtr());

	historicalBarCallbacks[oid] = callback;

}

void IBInterface::requestRealTimeTicks(string ticker, function<void(const Tick&)> callback)
{
	OrderId oid;
	if (stockTickOrderIds.find(ticker) != stockTickOrderIds.end())
	{
		oid = stockTickOrderIds[ticker];
	}
	else
	{
		oid = m_orderId;
		m_orderId++;
		stockTickOrderIds[ticker] = oid;

		//
		// Submit the request with historical data with the update option 
		// on
		//
		m_pClient->reqTickByTickData(
			oid,
			createUsStockContract(ticker),
			"AllLast",
			0,
			false
			);

	}
	stockTickCallbacks[oid].push_back(callback);
}

Contract IBInterface::createUsStockContract(string ticker)
{
	Contract c;
	c.symbol = ticker;
	c.secType = "STK";
	c.currency = "USD";
	c.exchange = "SMART";
	return c;
}

//! [contractdetailsend]
void IBInterface::contractDetailsEnd(int reqId) {
	printf("ContractDetailsEnd. %d\n", reqId);
}
//! [contractdetailsend]

//! [execdetails]
void IBInterface::execDetails(int reqId, const Contract& contract, const Execution& execution) {
	printf("ExecDetails. ReqId: %d - %s, %s, %s - %s, %ld, %g, %d\n", reqId, contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), execution.execId.c_str(), execution.orderId, execution.shares, execution.lastLiquidity);
}
//! [execdetails]

//! [execdetailsend]
void IBInterface::execDetailsEnd(int reqId) {
	printf("ExecDetailsEnd. %d\n", reqId);
}
//! [execdetailsend]

//! [updatemktdepth]
void IBInterface::updateMktDepth(TickerId id, int position, int operation, int side,
	double price, int size) {
	printf("UpdateMarketDepth. %ld - Position: %d, Operation: %d, Side: %d, Price: %g, Size: %d\n", id, position, operation, side, price, size);
}
//! [updatemktdepth]

//! [updatemktdepthl2]
void IBInterface::updateMktDepthL2(TickerId id, int position, const std::string& marketMaker, int operation,
	int side, double price, int size) {
	printf("UpdateMarketDepthL2. %ld - Position: %d, Operation: %d, Side: %d, Price: %g, Size: %d\n", id, position, operation, side, price, size);
}
//! [updatemktdepthl2]

//! [updatenewsbulletin]
void IBInterface::updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch) {
	printf("News Bulletins. %d - Type: %d, Message: %s, Exchange of Origin: %s\n", msgId, msgType, newsMessage.c_str(), originExch.c_str());
}
//! [updatenewsbulletin]

//! [managedaccounts]
void IBInterface::managedAccounts(const std::string& accountsList) {
	printf("Account List: %s\n", accountsList.c_str());
}
//! [managedaccounts]

//! [receivefa]
void IBInterface::receiveFA(faDataType pFaDataType, const std::string& cxml) {
	std::cout << "Receiving FA: " << (int)pFaDataType << std::endl << cxml << std::endl;
}
//! [receivefa]

//! [historicaldata]
void IBInterface::historicalData(TickerId reqId, const Bar& bar) {
	//printf("HistoricalData. ReqId: %ld - Date: %s, Open: %g, High: %g, Low: %g, Close: %g, Volume: %lld, Count: %d, WAP: %g\n", reqId, bar.time.c_str(), bar.open, bar.high, bar.low, bar.close, bar.volume, bar.count, bar.wap);
	//
	// If a function is requesting historical Data callback
	//
	if (historicalBarCallbacks.find(reqId) != historicalBarCallbacks.end())
	{
		historicalBarCallbacks[reqId](bar);
	}
}
//! [historicaldata]

//! [historicaldataend]
void IBInterface::historicalDataEnd(int reqId, const std::string& startDateStr, const std::string& endDateStr) {
	//std::cout << "HistoricalDataEnd. ReqId: " << reqId << " - Start Date: " << startDateStr << ", End Date: " << endDateStr << std::endl;
}
//! [historicaldataend]

//! [scannerparameters]
void IBInterface::scannerParameters(const std::string& xml) {
	printf("ScannerParameters. %s\n", xml.c_str());
}
//! [scannerparameters]

//! [scannerdata]
void IBInterface::scannerData(int reqId, int rank, const ContractDetails& contractDetails,
	const std::string& distance, const std::string& benchmark, const std::string& projection,
	const std::string& legsStr) {
	printf("ScannerData. %d - Rank: %d, Symbol: %s, SecType: %s, Currency: %s, Distance: %s, Benchmark: %s, Projection: %s, Legs String: %s\n", reqId, rank, contractDetails.contract.symbol.c_str(), contractDetails.contract.secType.c_str(), contractDetails.contract.currency.c_str(), distance.c_str(), benchmark.c_str(), projection.c_str(), legsStr.c_str());
}
//! [scannerdata]

//! [scannerdataend]
void IBInterface::scannerDataEnd(int reqId) {
	printf("ScannerDataEnd. %d\n", reqId);
}
//! [scannerdataend]

//! [realtimebar]
void IBInterface::realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
	long volume, double wap, int count) {
	printf("RealTimeBars. %ld - Time: %ld, Open: %g, High: %g, Low: %g, Close: %g, Volume: %ld, Count: %d, WAP: %g\n", reqId, time, open, high, low, close, volume, count, wap);
}
//! [realtimebar]

//! [fundamentaldata]
void IBInterface::fundamentalData(TickerId reqId, const std::string& data) {
	printf("FundamentalData. ReqId: %ld, %s\n", reqId, data.c_str());
}
//! [fundamentaldata]

void IBInterface::deltaNeutralValidation(int reqId, const DeltaNeutralContract& deltaNeutralContract) {
	printf("DeltaNeutralValidation. %d, ConId: %ld, Delta: %g, Price: %g\n", reqId, deltaNeutralContract.conId, deltaNeutralContract.delta, deltaNeutralContract.price);
}

//! [ticksnapshotend]
void IBInterface::tickSnapshotEnd(int reqId) {
	printf("TickSnapshotEnd: %d\n", reqId);
}
//! [ticksnapshotend]

//! [marketdatatype]
void IBInterface::marketDataType(TickerId reqId, int marketDataType) {
	printf("MarketDataType. ReqId: %ld, Type: %d\n", reqId, marketDataType);
}
//! [marketdatatype]

//! [commissionreport]
void IBInterface::commissionReport(const CommissionReport& commissionReport) {
	printf("CommissionReport. %s - %g %s RPNL %g\n", commissionReport.execId.c_str(), commissionReport.commission, commissionReport.currency.c_str(), commissionReport.realizedPNL);
}
//! [commissionreport]

//! [position]
void IBInterface::position(const std::string& account, const Contract& contract, double position, double avgCost) {
	printf("Position. %s - Symbol: %s, SecType: %s, Currency: %s, Position: %g, Avg Cost: %g\n", account.c_str(), contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), position, avgCost);
}
//! [position]

//! [positionend]
void IBInterface::positionEnd() {
	printf("PositionEnd\n");
}
//! [positionend]

//! [accountsummary]
void IBInterface::accountSummary(int reqId, const std::string& account, const std::string& tag, const std::string& value, const std::string& currency) {
	printf("Acct Summary. ReqId: %d, Account: %s, Tag: %s, Value: %s, Currency: %s\n", reqId, account.c_str(), tag.c_str(), value.c_str(), currency.c_str());
}
//! [accountsummary]

//! [accountsummaryend]
void IBInterface::accountSummaryEnd(int reqId) {
	printf("AccountSummaryEnd. Req Id: %d\n", reqId);
}
//! [accountsummaryend]

void IBInterface::verifyMessageAPI(const std::string& apiData) {
	printf("verifyMessageAPI: %s\b", apiData.c_str());
}

void IBInterface::verifyCompleted(bool isSuccessful, const std::string& errorText) {
	printf("verifyCompleted. IsSuccessfule: %d - Error: %s\n", isSuccessful, errorText.c_str());
}

void IBInterface::verifyAndAuthMessageAPI(const std::string& apiDatai, const std::string& xyzChallenge) {
	printf("verifyAndAuthMessageAPI: %s %s\n", apiDatai.c_str(), xyzChallenge.c_str());
}

void IBInterface::verifyAndAuthCompleted(bool isSuccessful, const std::string& errorText) {
	printf("verifyAndAuthCompleted. IsSuccessful: %d - Error: %s\n", isSuccessful, errorText.c_str());
	if (isSuccessful)
		m_pClient->startApi();
}

//! [displaygrouplist]
void IBInterface::displayGroupList(int reqId, const std::string& groups) {
	printf("Display Group List. ReqId: %d, Groups: %s\n", reqId, groups.c_str());
}
//! [displaygrouplist]

//! [displaygroupupdated]
void IBInterface::displayGroupUpdated(int reqId, const std::string& contractInfo) {
	std::cout << "Display Group Updated. ReqId: " << reqId << ", Contract Info: " << contractInfo << std::endl;
}
//! [displaygroupupdated]

//! [positionmulti]
void IBInterface::positionMulti(int reqId, const std::string& account, const std::string& modelCode, const Contract& contract, double pos, double avgCost) {
	printf("Position Multi. Request: %d, Account: %s, ModelCode: %s, Symbol: %s, SecType: %s, Currency: %s, Position: %g, Avg Cost: %g\n", reqId, account.c_str(), modelCode.c_str(), contract.symbol.c_str(), contract.secType.c_str(), contract.currency.c_str(), pos, avgCost);
}
//! [positionmulti]

//! [positionmultiend]
void IBInterface::positionMultiEnd(int reqId) {
	printf("Position Multi End. Request: %d\n", reqId);
}
//! [positionmultiend]

//! [accountupdatemulti]
void IBInterface::accountUpdateMulti(int reqId, const std::string& account, const std::string& modelCode, const std::string& key, const std::string& value, const std::string& currency) {
	printf("AccountUpdate Multi. Request: %d, Account: %s, ModelCode: %s, Key, %s, Value: %s, Currency: %s\n", reqId, account.c_str(), modelCode.c_str(), key.c_str(), value.c_str(), currency.c_str());
}
//! [accountupdatemulti]

//! [accountupdatemultiend]
void IBInterface::accountUpdateMultiEnd(int reqId) {
	printf("Account Update Multi End. Request: %d\n", reqId);
}
//! [accountupdatemultiend]

//! [securityDefinitionOptionParameter]
void IBInterface::securityDefinitionOptionalParameter(int reqId, const std::string& exchange, int underlyingConId, const std::string& tradingClass,
	const std::string& multiplier, const std::set<std::string>& expirations, const std::set<double>& strikes) {
	printf("Security Definition Optional Parameter. Request: %d, Trading Class: %s, Multiplier: %s\n", reqId, tradingClass.c_str(), multiplier.c_str());
}
//! [securityDefinitionOptionParameter]

//! [securityDefinitionOptionParameterEnd]
void IBInterface::securityDefinitionOptionalParameterEnd(int reqId) {
	printf("Security Definition Optional Parameter End. Request: %d\n", reqId);
}
//! [securityDefinitionOptionParameterEnd]

//! [softDollarTiers]
void IBInterface::softDollarTiers(int reqId, const std::vector<SoftDollarTier> &tiers) {
	printf("Soft dollar tiers (%lu):", tiers.size());

	for (unsigned int i = 0; i < tiers.size(); i++) {
		printf("%s\n", tiers[i].displayName().c_str());
	}
}
//! [softDollarTiers]

//! [familyCodes]
void IBInterface::familyCodes(const std::vector<FamilyCode> &familyCodes) {
	printf("Family codes (%lu):\n", familyCodes.size());

	for (unsigned int i = 0; i < familyCodes.size(); i++) {
		printf("Family code [%d] - accountID: %s familyCodeStr: %s\n", i, familyCodes[i].accountID.c_str(), familyCodes[i].familyCodeStr.c_str());
	}
}
//! [familyCodes]

//! [symbolSamples]
void IBInterface::symbolSamples(int reqId, const std::vector<ContractDescription> &contractDescriptions) {
	printf("Symbol Samples (total=%lu) reqId: %d\n", contractDescriptions.size(), reqId);

	for (unsigned int i = 0; i < contractDescriptions.size(); i++) {
		Contract contract = contractDescriptions[i].contract;
		std::vector<std::string> derivativeSecTypes = contractDescriptions[i].derivativeSecTypes;
		printf("Contract (%u): %ld %s %s %s %s, ", i, contract.conId, contract.symbol.c_str(), contract.secType.c_str(), contract.primaryExchange.c_str(), contract.currency.c_str());
		printf("Derivative Sec-types (%lu):", derivativeSecTypes.size());
		for (unsigned int j = 0; j < derivativeSecTypes.size(); j++) {
			printf(" %s", derivativeSecTypes[j].c_str());
		}
		printf("\n");
	}
}
//! [symbolSamples]

//! [mktDepthExchanges]
void IBInterface::mktDepthExchanges(const std::vector<DepthMktDataDescription> &depthMktDataDescriptions) {
	printf("Mkt Depth Exchanges (%lu):\n", depthMktDataDescriptions.size());

	for (unsigned int i = 0; i < depthMktDataDescriptions.size(); i++) {
		printf("Depth Mkt Data Description [%d] - exchange: %s secType: %s listingExch: %s serviceDataType: %s aggGroup: %s\n", i,
			depthMktDataDescriptions[i].exchange.c_str(),
			depthMktDataDescriptions[i].secType.c_str(),
			depthMktDataDescriptions[i].listingExch.c_str(),
			depthMktDataDescriptions[i].serviceDataType.c_str(),
			depthMktDataDescriptions[i].aggGroup != INT_MAX ? std::to_string(depthMktDataDescriptions[i].aggGroup).c_str() : "");
	}
}
//! [mktDepthExchanges]

//! [tickNews]
void IBInterface::tickNews(int tickerId, time_t timeStamp, const std::string& providerCode, const std::string& articleId, const std::string& headline, const std::string& extraData) {
	printf("News Tick. TickerId: %d, TimeStamp: %s, ProviderCode: %s, ArticleId: %s, Headline: %s, ExtraData: %s\n", tickerId, ctime(&(timeStamp /= 1000)), providerCode.c_str(), articleId.c_str(), headline.c_str(), extraData.c_str());
}
//! [tickNews]

//! [smartcomponents]]
void IBInterface::smartComponents(int reqId, const SmartComponentsMap& theMap) {
	printf("Smart components: (%lu):\n", theMap.size());

	for (SmartComponentsMap::const_iterator i = theMap.begin(); i != theMap.end(); i++) {
		printf(" bit number: %d exchange: %s exchange letter: %c\n", i->first, std::get<0>(i->second).c_str(), std::get<1>(i->second));
	}
}
//! [smartcomponents]

//! [tickReqParams]
void IBInterface::tickReqParams(int tickerId, double minTick, const std::string& bboExchange, int snapshotPermissions) {
	printf("tickerId: %d, minTick: %g, bboExchange: %s, snapshotPermissions: %u", tickerId, minTick, bboExchange.c_str(), snapshotPermissions);

	m_bboExchange = bboExchange;
}
//! [tickReqParams]

//! [newsProviders]
void IBInterface::newsProviders(const std::vector<NewsProvider> &newsProviders) {
	printf("News providers (%lu):\n", newsProviders.size());

	for (unsigned int i = 0; i < newsProviders.size(); i++) {
		printf("News provider [%d] - providerCode: %s providerName: %s\n", i, newsProviders[i].providerCode.c_str(), newsProviders[i].providerName.c_str());
	}
}
//! [newsProviders]

//! [newsArticle]
void IBInterface::newsArticle(int requestId, int articleType, const std::string& articleText) {
//	printf("News Article. Request Id: %d, Article Type: %d\n", requestId, articleType);
//	if (articleType == 0) {
//		printf("News Article Text (text or html): %s\n", articleText.c_str());
//	}
//	else if (articleType == 1) {
//		std::string path;
//#if defined(IB_WIN32)
//		TCHAR s[200];
//		GetCurrentDirectory(200, s);
//		path = s + std::string("\\MST$06f53098.pdf");
//#elif defined(IB_POSIX)
//		char s[1024];
//		if (getcwd(s, sizeof(s)) == NULL) {
//			printf("getcwd() error\n");
//			return;
//		}
//		path = s + std::string("/MST$06f53098.pdf");
//#endif
//		std::vector<std::uint8_t> bytes = Utils::base64_decode(articleText);
//		std::ofstream outfile(path, std::ios::out | std::ios::binary);
//		outfile.write((const char*)bytes.data(), bytes.size());
//		printf("Binary/pdf article was saved to: %s\n", path.c_str());
//	}
}
//! [newsArticle]

//! [historicalNews]
void IBInterface::historicalNews(int requestId, const std::string& time, const std::string& providerCode, const std::string& articleId, const std::string& headline) {
	printf("Historical News. RequestId: %d, Time: %s, ProviderCode: %s, ArticleId: %s, Headline: %s\n", requestId, time.c_str(), providerCode.c_str(), articleId.c_str(), headline.c_str());
}
//! [historicalNews]

//! [historicalNewsEnd]
void IBInterface::historicalNewsEnd(int requestId, bool hasMore) {
	printf("Historical News End. RequestId: %d, HasMore: %s\n", requestId, (hasMore ? "true" : " false"));
}
//! [historicalNewsEnd]

//! [headTimestamp]
void IBInterface::headTimestamp(int reqId, const std::string& headTimestamp) {
	printf("Head time stamp. ReqId: %d - Head time stamp: %s,\n", reqId, headTimestamp.c_str());

}
//! [headTimestamp]

//! [histogramData]
void IBInterface::histogramData(int reqId, const HistogramDataVector& data) {
	printf("Histogram. ReqId: %d, data length: %lu\n", reqId, data.size());

	for (auto item : data) {
		printf("\t price: %f, size: %lld\n", item.price, item.size);
	}
}
//! [histogramData]

//! [historicalDataUpdate]
void IBInterface::historicalDataUpdate(TickerId reqId, const Bar& bar) {
	//printf("HistoricalDataUpdate. ReqId: %ld - Date: %s, Open: %g, High: %g, Low: %g, Close: %g, Volume: %lld, Count: %d, WAP: %g\n", reqId, bar.time.c_str(), bar.open, bar.high, bar.low, bar.close, bar.volume, bar.count, bar.wap);
	//
	//
	// Traverse the callbacks registered to this ticker
	// For each index, check to see if a new candle should be sent
	//

	Callback& orderIdCallback = stockRealTimeBarCallbacks[reqId];

	// Parses the time for the minutes of a bar in integer;
	//
	auto getPrevBarMinutes = [&]()
	{
		return stoi(orderIdCallback.callbackBar.time.substr(13, 2));
	};

	//
	// If the bar time has changed (compared to a non new bar) and the 
	// previous bar time was a multiple of the current timeframe, then 
	// the previous bar was complete and should be dispatched via the 
	// callback.
	//
	if (orderIdCallback.callbackBar.time.length() != 0 &&
		bar.time != orderIdCallback.callbackBar.time &&
		getPrevBarMinutes() % orderIdCallback.timeFrame == 0)
	{
		for (auto& fn : orderIdCallback.callbackFunctions)
		{
			fn(orderIdCallback.callbackBar);
		}
	}

	orderIdCallback.callbackBar = bar;
	
}
//! [historicalDataUpdate]

//! [rerouteMktDataReq]
void IBInterface::rerouteMktDataReq(int reqId, int conid, const std::string& exchange) {
	printf("Re-route market data request. ReqId: %d, ConId: %d, Exchange: %s\n", reqId, conid, exchange.c_str());
}
//! [rerouteMktDataReq]

//! [rerouteMktDepthReq]
void IBInterface::rerouteMktDepthReq(int reqId, int conid, const std::string& exchange) {
	printf("Re-route market depth request. ReqId: %d, ConId: %d, Exchange: %s\n", reqId, conid, exchange.c_str());
}
//! [rerouteMktDepthReq]

//! [marketRule]
void IBInterface::marketRule(int marketRuleId, const std::vector<PriceIncrement> &priceIncrements) {
	printf("Market Rule Id: %d\n", marketRuleId);
	for (unsigned int i = 0; i < priceIncrements.size(); i++) {
		printf("Low Edge: %g, Increment: %g\n", priceIncrements[i].lowEdge, priceIncrements[i].increment);
	}
}
//! [marketRule]

//! [pnl]
void IBInterface::pnl(int reqId, double dailyPnL, double unrealizedPnL, double realizedPnL) {
	printf("PnL. ReqId: %d, daily PnL: %g, unrealized PnL: %g, realized PnL: %g\n", reqId, dailyPnL, unrealizedPnL, realizedPnL);
}
//! [pnl]

//! [pnlsingle]
void IBInterface::pnlSingle(int reqId, int pos, double dailyPnL, double unrealizedPnL, double realizedPnL, double value) {
	printf("PnL Single. ReqId: %d, pos: %d, daily PnL: %g, unrealized PnL: %g, realized PnL: %g, value: %g\n", reqId, pos, dailyPnL, unrealizedPnL, realizedPnL, value);
}
//! [pnlsingle]

//! [historicalticks]
void IBInterface::historicalTicks(int reqId, const std::vector<HistoricalTick>& ticks, bool done) {
	for (HistoricalTick tick : ticks) {
		std::time_t t = tick.time;
		std::cout << "Historical tick. ReqId: " << reqId << ", time: " << ctime(&t) << ", price: " << tick.price << ", size: " << tick.size << std::endl;
	}
}
//! [historicalticks]

//! [historicalticksbidask]
void IBInterface::historicalTicksBidAsk(int reqId, const std::vector<HistoricalTickBidAsk>& ticks, bool done) {
	for (HistoricalTickBidAsk tick : ticks) {
		std::time_t t = tick.time;
		std::cout << "Historical tick bid/ask. ReqId: " << reqId << ", time: " << ctime(&t) << ", mask: " << tick.mask << ", price bid: " << tick.priceBid <<
			", price ask: " << tick.priceAsk << ", size bid: " << tick.sizeBid << ", size ask: " << tick.sizeAsk << std::endl;
	}
}
//! [historicalticksbidask]

//! [historicaltickslast]
void IBInterface::historicalTicksLast(int reqId, const std::vector<HistoricalTickLast>& ticks, bool done) {
	for (HistoricalTickLast tick : ticks) {
		std::time_t t = tick.time;
		std::cout << "Historical tick last. ReqId: " << reqId << ", time: " << ctime(&t) << ", mask: " << tick.mask << ", price: " << tick.price <<
			", size: " << tick.size << ", exchange: " << tick.exchange << ", special conditions: " << tick.specialConditions << std::endl;
	}
}
//! [historicaltickslast]

//! [tickbytickalllast]
void IBInterface::tickByTickAllLast(int reqId, int tickType, time_t time, double price, int size, const TickAttrib& attribs, const std::string& exchange, const std::string& specialConditions) {

	Tick t = {tickType, time, price, size, attribs, exchange};

	for (auto& fn : stockTickCallbacks[reqId])
	{
		fn(t);
	}
}
//! [tickbytickalllast]

//! [tickbytickbidask]
void IBInterface::tickByTickBidAsk(int reqId, time_t time, double bidPrice, double askPrice, int bidSize, int askSize, const TickAttrib& attribs) {
	printf("Tick-By-Tick. ReqId: %d, TickType: BidAsk, Time: %s, BidPrice: %g, AskPrice: %g, BidSize: %d, AskSize: %d, BidPastLow: %d, AskPastHigh: %d\n",
		reqId, ctime(&time), bidPrice, askPrice, bidSize, askSize, attribs.bidPastLow, attribs.askPastHigh);
}
//! [tickbytickbidask]

//! [tickbytickmidpoint]
void IBInterface::tickByTickMidPoint(int reqId, time_t time, double midPoint) {
	printf("Tick-By-Tick. ReqId: %d, TickType: MidPoint, Time: %s, MidPoint: %g\n", reqId, ctime(&time), midPoint);
}
//! [tickbytickmidpoint]
