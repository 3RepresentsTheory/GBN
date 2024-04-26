add_executable(
        GBNPDU_test
        GBNPDU_test.cpp
        ../src/GBNReceiver.cpp
        ../src/GBNSender.cpp
        ../src/GBNSocket.cpp
        ../src/GBNPDU.cpp
)


add_executable(
        GBNPDU_epoll_test
        simple_epoll_echo_server.cpp
)