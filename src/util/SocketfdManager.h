//
// Created by no3core on 2024/4/26.
//

#ifndef GOBACKN_SOCKETFDMANAGER_H
#define GOBACKN_SOCKETFDMANAGER_H

#include <cstdint>
#include <stdexcept>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

/*
 * SocketfdManager: generate listening udp socket
 */
constexpr static uint64_t DEFAULT_PORT = 12345;

class SocketfdManager{
private:
    SocketfdManager(const SocketfdManager&other) = delete;
    SocketfdManager&operator=(const SocketfdManager&other) = delete;

    int fd_;
public:
    SocketfdManager(uint64_t udp_port = DEFAULT_PORT) {
        fd_ = socket(AF_INET,SOCK_DGRAM,0);
        if(fd_<0)
            throw std::runtime_error("cannot create socket fd");

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(udp_port);

        if (bind(fd_, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
            close(fd_);
            throw std::runtime_error("cannot create bind udp port");
        }
    }
    ~SocketfdManager(){ close(fd_); }
    int GetFd() const {return fd_;}
};

#endif //GOBACKN_SOCKETFDMANAGER_H
