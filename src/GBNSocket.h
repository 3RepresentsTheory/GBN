//
// Created by no3core on 2024/4/17.
//

#ifndef GOBACKN_GBNSOCKET_H
#define GOBACKN_GBNSOCKET_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include "GBNConnection.h"
#include "util/EventLoop.h"
#include "util/TimerfdManager.h"
#include "util/SocketfdManager.h"
#include "util/PipefdManager.h"

class GBNSocket{

private:

    TimerfdManager  tfd_;
    SocketfdManager sfd_;
    PipefdManager   pfd_;

    GBNConnection connection_;

    sockaddr_in peer_addr_;

    uint16_t    local_port_;

    std::mutex     read_mtx_;
    std::condition_variable read_cv_;

    std::thread     eventthread_;
    EventLoop       eventloop_;


    void FrameReceived(int socketfd);
    void FrameSent(int eventfd);

public:
    GBNSocket(
            uint16_t local_port
    );

    ~GBNSocket();

    size_t Write(const std::string & str);
    size_t Write(const char*buf, size_t n);
    size_t Read(char*buf ,size_t n);


};

#endif //GOBACKN_GBNSOCKET_H
