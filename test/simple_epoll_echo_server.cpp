//
// Created by no3core on 2024/4/25.
//
#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include "../src/util/EventLoop.h"
#include "../src/util/TimerfdManager.h"
#include "../src/util/SocketfdManager.h"

constexpr int MAX_EVENTS = 10;
constexpr int BUFFER_SIZE = 1024;
constexpr int PORT = 12345;

int main() {
    EventLoop eventLoop;

    // listening udp socket
    SocketfdManager socketfdManager;
    int sockfd = socketfdManager.GetFd();

    // time trigger
    TimerfdManager timerfdManager;
    int timerfd = timerfdManager.GetFd();

    eventLoop.RegisterEvent(
            sockfd,
            EPOLLIN,
            [sockfd](){
                std::cout << "udp package received!" <<std::endl;
                char buffer[BUFFER_SIZE];
                sockaddr_in clientAddr{};
                socklen_t addrLen = sizeof(clientAddr);
                ssize_t bytesRead = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                                             reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
                if (bytesRead < 0) {
                    std::cerr << "Failed to receive data." << std::endl;
                    return ;
                }

                // 发送回客户端
                ssize_t bytesSent = sendto(sockfd, buffer, bytesRead, 0,
                                           reinterpret_cast<sockaddr*>(&clientAddr), addrLen);
                if (bytesSent < 0) {
                    std::cerr << "Failed to send data." << std::endl;
                    return;
                }
            }
    );


    eventLoop.RegisterEvent(
            timerfd,
            EPOLLIN | EPOLLET,
            [timerfd](){
                static uint tid = 0;
                uint64_t expiration = 0;
                read(timerfd,&expiration,sizeof(expiration));
                if(expiration>1)
                    std::cout << tid++ << " time ticked "<<expiration << std::endl;
            }
    );

    eventLoop.Loop();

    return 0;
}