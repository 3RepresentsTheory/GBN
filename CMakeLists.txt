cmake_minimum_required(VERSION 3.27)
project(gobackn)

set(CMAKE_CXX_STANDARD 14)

add_executable(gobackn
        main.cpp
        src/GBNReceiver.cpp
        src/GBNSender.cpp
        src/GBNSocket.cpp
        src/GBNPDU.cpp
)


add_executable(test
        test/GBNPDU_test.cpp
    src/GBNReceiver.cpp
    src/GBNSender.cpp
    src/GBNSocket.cpp
    src/GBNPDU.cpp
)
