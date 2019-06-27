cmake_minimum_required (VERSION 3.15.0)
 
# Maps to Visual Studio solution file (Tutorial.sln)
# The solution will have all targets (exe, lib, dll) 
# as Visual Studio projects (.vcproj)
project (InteractiveBrokersLib)
 
# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
 
# Set compiler flags and options. 
# Here it is setting the Visual Studio warning level to 4
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
 
# Command to output information to the console
# Useful for displaying errors, warnings, and debugging
message ("cxx Flags:" ${CMAKE_CXX_FLAGS})

add_library(InteractiveBrokersLib STATIC
./AccountSummaryTags.h
./AvailableAlgoParams.h
./bar.h
./CommissionReport.h
./CommonDefs.h
./Contract.h
./ContractSamples.h
./ContractCondition.h
./DefaultEWrapper.h
./DepthMktDataDescription.h
./EClient.h
./EClientMsgSink.h
./EClientSocket.h
./EDecoder.h
./EMessage.h
./EMutex.h
./EReader.h
./EReaderOSSignal.h
./EReaderSignal.h
./EReaderWMSignal.h
./EPosixClientSocketPlatform.h
./ESocket.h
./ETransport.h
./EWrapper.h
./EWrapper_prototypes.h
./Execution.h
./executioncondition.h
./FAMethodSamples.h
./FamilyCode.h
./HistogramEntry.h
./HistoricalTick.h
./HistoricalTickBidAsk.h
./HistoricalTickLast.h
./IExternalizable.h
./InteractiveBrokersApi.h
./MarginCondition.h
./NewsProvider.h
./OrderCondition.h
./OperatorCondition.h
./Order.h
./OrderSamples.h
./OrderState.h
./PriceCondition.h
./PriceIncrement.h
./PercentChangeCondition.h
./SoftDollarTier.h
./StdAfx.h
./TagValue.h
./targetver.h
./Tick.h
./TickAttrib.h
./ScannerSubscriptionSamples.h
./Resource.h
./ScannerSubscription.h
./shared_ptr.h
./TimeCondition.h
./TwsSocketClientErrors.h
./VolumeCondition.h
./Utils.h
./AccountSummaryTags.cpp
./AvailableAlgoParams.cpp
./ContractCondition.cpp
./ContractSamples.cpp
./DefaultEWrapper.cpp
./EClient.cpp
./EClientSocket.cpp
./EDecoder.cpp
./EMessage.cpp
./EMutex.cpp
./EReader.cpp
./EReaderOSSignal.cpp
./EReaderWMSignal.cpp
./ESocket.cpp
./executioncondition.cpp
./InteractiveBrokersApi.cpp
./MarginCondition.cpp
./OperatorCondition.cpp
./OrderCondition.cpp
./OrderSamples.cpp
./PercentChangeCondition.cpp
./PriceCondition.cpp
./ScannerSubscriptionSamples.cpp
./SoftDollarTier.cpp
./TimeCondition.cpp
./VolumeCondition.cpp
)
