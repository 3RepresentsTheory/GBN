cmake_minimum_required(VERSION 3.22)
project(gobackn)

set(CMAKE_CXX_STANDARD 14)

# third party json library for config parse
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

add_subdirectory(test)
add_subdirectory(src)
add_subdirectory(bin)

