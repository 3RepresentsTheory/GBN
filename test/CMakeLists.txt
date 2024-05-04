
add_executable( GBNPDU_test
    GBNPDU_test.cpp
    ../src/GBNReceiver.cpp
    ../src/GBNSender.cpp
    ../src/GBNSocket.cpp
    ../src/GBNPDU.cpp
    ../src/GBNByteStream.cpp
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

add_executable( GBNConnection_test
    GBNConnection_test.cpp
    GBNSenderTestHarness.cpp
    ../src/GBNConnection.cpp
    ../src/GBNSender.cpp
    ../src/GBNByteStream.cpp
    ../src/GBNPDU.cpp
)


add_executable( GBNSocket_test
    ../src/GBNSocket.cpp
)



add_executable( GBNSocketClient_test
    GBNSocketClient_test.cpp
    ../src/GBNSocket.cpp
    ../src/GBNConnection.cpp
    ../src/GBNSender.cpp
    ../src/GBNByteStream.cpp
    ../src/GBNPDU.cpp
)

add_executable( GBNFileReceiver_test
        GBNFileReceiver_test.cpp
        ../src/GBNSocket.cpp
        ../src/GBNConnection.cpp
        ../src/GBNSender.cpp
        ../src/GBNByteStream.cpp
        ../src/GBNPDU.cpp
)
add_executable( GBNFileSender_test
        GBNFileSender_test.cpp
        ../src/GBNSocket.cpp
        ../src/GBNConnection.cpp
        ../src/GBNSender.cpp
        ../src/GBNByteStream.cpp
        ../src/GBNPDU.cpp
)

#target_include_directories(
#        GBNSocketClient_test
#        PRIVATE
#        spdlog::spdlog_header_only
#)



add_executable( GBNSocketServer_test
        GBNSocketServer_test.cpp
        ../src/GBNSocket.cpp
        ../src/GBNConnection.cpp
        ../src/GBNSender.cpp
        ../src/GBNByteStream.cpp
        ../src/GBNPDU.cpp
)

#target_include_directories(
#        GBNSocketServer_test
#        PRIVATE
#        spdlog::spdlog_header_only
#)

add_executable(
        jsontest
        jsontest.cpp
)

target_link_libraries(jsontest PRIVATE nlohmann_json::nlohmann_json)

