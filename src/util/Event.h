//
// Created by no3core on 2024/4/25.
//

#ifndef GOBACKN_EVENT_H
#define GOBACKN_EVENT_H

#include <functional>
#include <cstdint>
#include <sys/epoll.h>

class Event{
private:
    std::function<void()> call_back_;
    uint64_t event_;
    int fd_;
public:
    Event(
       std::function<void()>call_back,
       int fd,
       uint64_t event = EPOLLIN | EPOLLET
    ){
        event_ = event;
        fd_    = fd;
        call_back_ = std::move(call_back);
    }
    void CallBack(){call_back_();}
    uint64_t GetEvent() const{return event_;}
    int GetFd() const { return fd_;}

};

#endif //GOBACKN_EVENT_H
