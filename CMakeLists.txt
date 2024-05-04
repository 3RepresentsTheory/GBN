cmake_minimum_required(VERSION 3.22)
project(gobackn)

set(CMAKE_CXX_STANDARD 14)

#add_executable(
#        gobackn
#        src/GBNReceiver.cpp
#        src/GBNSender.cpp
#        src/GBNSocket.cpp
#        src/GBNPDU.cpp
#)

#if(NOT TARGET spdlog)
#    # Stand-alone build
#    find_package(spdlog REQUIRED)
#endif()
#include_directories(SYSTEM "/mnt/e/programming/c++/gobackn/spdlog/include/spdlog")

add_subdirectory(test)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)


