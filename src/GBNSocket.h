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

#include <arpa/inet.h>

class GBNSocket{

private:

    TimerfdManager  tfd_;
    SocketfdManager sfd_;
    PipefdManager   pfd_;
    PipefdManager   closefd_;

    // state machine of socket request read&write
    enum RSTATUS{
        READY,
        WREPLY,
        WCHGE,
        PASS,
        RETRY
    };
    PipefdManager   rrequest_;
    PipefdManager   rreply_;

    std::condition_variable read_cv_;
    std::mutex      read_mt_;
    int rstate_ = RSTATUS::READY;

    PipefdManager   wrequest_;
    PipefdManager   wreply_;
    int wstate_ = RSTATUS::READY;

    std::condition_variable write_cv_;
    std::mutex      write_mt_;

    GBNConnection connection_;

    sockaddr_in peer_addr_;
    uint16_t    local_port_;

    std::thread     eventthread_;
    EventLoop       eventloop_;


    void FrameReceived(int socketfd);
    void FrameSent(int eventfd);

public:
    // create a listening socket
    GBNSocket(
            uint16_t local_port
    );

    // create a client socket
    GBNSocket(
            uint16_t local_port,
            uint16_t peer_port,
            const std::string& peer_ip
    ): GBNSocket(local_port){
        peer_addr_.sin_family = AF_INET;
        peer_addr_.sin_port   = htons(peer_port);
        if(inet_pton(AF_INET,peer_ip.c_str(),&(peer_addr_.sin_addr))<0)
            throw std::runtime_error("invalid peer address");
    }

    ~GBNSocket();

    size_t Write(const std::string & str);
    size_t Write(const char*buf, size_t n);
    size_t Read(char*buf ,size_t n);
    std::string Read(size_t n);

    void Close();


};

#endif //GOBACKN_GBNSOCKET_H
