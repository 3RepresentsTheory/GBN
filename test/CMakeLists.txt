add_executable( GBNPDU_test
    GBNPDU_test.cpp
    ../src/GBNReceiver.cpp
    ../src/GBNSender.cpp
    ../src/GBNSocket.cpp
    ../src/GBNPDU.cpp
)

add_executable( GBNPDU_epoll_test
    simple_epoll_echo_server.cpp
)

add_executable( ByteStream_test
    ByteStream_test.cpp
    ../src/GBNByteStream.cpp
    ../src/GBNPDU.cpp
)

add_executable( GBNReceiver_test
    GBNReceiver_test.cpp
    ../src/GBNByteStream.cpp
    ../src/GBNPDU.cpp
)

add_executable( GBNSender_test
    GBNSender_test.cpp
    GBNSenderTestHarness.cpp
    ../src/GBNSender.cpp
    ../src/GBNByteStream.cpp
    ../src/GBNPDU.cpp
)