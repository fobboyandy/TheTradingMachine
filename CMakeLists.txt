cmake_minimum_required (VERSION 3.15.0)
 
# Maps to Visual Studio solution file (Tutorial.sln)
# The solution will have all targets (exe, lib, dll) 
# as Visual Studio projects (.vcproj)
project (TheTradingMachineComponents)
 
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

include_directories(include external/InteractiveBrokersApi/)
LINK_DIRECTORIES(external/lib/Debug)

add_library(BaseAlgorithm SHARED
./include/BaseAlgorithm.h
./include/LocalBroker.h
./include/Portfolio.h
./include/Position.h
./include/PlotData.h
./include/TickBroadcast.h
./src/BaseAlgorithm.cpp
./src/LocalBroker.cpp
./src/Portfolio.cpp
./src/TickBroadcast.cpp
)

target_link_libraries(BaseAlgorithm InteractiveBrokersClient)

add_library(InteractiveBrokersClient SHARED
./include/InteractiveBrokersClient.h
./src/InteractiveBrokersClient.cpp
)

target_link_libraries(InteractiveBrokersClient InteractiveBrokersLib)
