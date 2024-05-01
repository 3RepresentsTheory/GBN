//
// Created by no3core on 2024/4/25.
//

#ifndef GOBACKN_TIMERFDMANAGER_H
#define GOBACKN_TIMERFDMANAGER_H

#include <sys/timerfd.h>
#include <unistd.h>
#include <cstdint>
#include <stdexcept>

// TODO: more friendly interval control
constexpr static uint64_t INTERVAL = 100; //ms
// 1s = 1000 ms = 1000 000 000ns

/*
 * TimerfdManager: generate timer fd for event loop
 */
class TimerfdManager{
private:
    // cannot copy the timer fd
    TimerfdManager(const TimerfdManager&other) = delete;
    TimerfdManager&operator=(const TimerfdManager&other) = delete;

    int fd_;
    uint64_t interval_;
public:
    // mini_interval is by ms
    TimerfdManager(uint64_t mini_interval = INTERVAL){
        fd_ = timerfd_create(CLOCK_REALTIME,0);
        if(fd_<0)
            throw std::runtime_error("cannot start timer fd");

        interval_ = mini_interval;
        struct itimerspec timer;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_nsec = mini_interval*1000000;
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_nsec = mini_interval*1000000;

        if (timerfd_settime(fd_, 0, &timer, nullptr) == -1){
            setbuf(stdout,0);
            throw std::runtime_error("cannot set timer fd");
        }
    }
    ~TimerfdManager(){
        struct itimerspec pseudo ;
        timerfd_settime(fd_, 0, &pseudo, nullptr);
        close(fd_);
    }
    int GetFd() const { return fd_; }
    uint64_t GetInterval() const{ return interval_;}
};




#endif //GOBACKN_TIMERFDMANAGER_H
