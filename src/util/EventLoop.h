//
// Created by no3core on 2024/4/25.
//

#ifndef GOBACKN_EVENTLOOP_H
#define GOBACKN_EVENTLOOP_H

#include <sys/epoll.h>
#include <stdexcept>
#include <csignal>
#include <set>
#include <vector>
#include <cstring>
#include "Event.h"


class EventLoop{
private:
    static constexpr int MAX_EVENTS = 10;
    int epollfd_;
    int max_events_;
    volatile int is_running_ = true;
    std::vector<Event> events_;
    epoll_event ready_events_[];

public:
    EventLoop(int max_event = MAX_EVENTS){
        max_events_ = max_event;
        epollfd_ = epoll_create1(0);
        if(epollfd_<0)
            throw std::runtime_error("cannot create epoll fd");
    }
    ~EventLoop(){ close(epollfd_); }

    void RegisterEvent(
            int fd,
            uint64_t event_flag,
            const std::function<void()> & callback
    ){
        events_.emplace_back(callback,fd,event_flag);

        epoll_event epollevent{};
        epollevent.events  = events_.back().GetEvent();
        epollevent.data.u32= events_.size()-1;

        if(epoll_ctl(epollfd_,EPOLL_CTL_ADD,fd,&epollevent)<0){
            close(fd);
            close(epollfd_);
            throw std::runtime_error("cannot register event");
        }
    }

    void Loop(){
        while(is_running_){
            // try to retrieve available event
            int nevents = epoll_wait(epollfd_,ready_events_,max_events_,-1);
            if(nevents<0){
                char *estr = strerror(errno);
                throw std::runtime_error(
                        "failed to wait event with error: "+
                        std::string(estr,strlen(estr))
                );

            }

            // find ready event and execute callback
            for (int i = 0; i < nevents; ++i) {
                Event &event_p = events_[ready_events_[i].data.u32];
                event_p.CallBack();
            }
        }
    }

    void StopLoop(){is_running_ = false;}

};

#endif //GOBACKN_EVENTLOOP_H
