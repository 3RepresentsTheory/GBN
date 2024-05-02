//
// Created by no3core on 2024/4/17.
//

#ifndef GOBACKN_GBNSENDER_H
#define GOBACKN_GBNSENDER_H


#include <cstdint>
#include <stdexcept>
#include <functional>
#include "GBNByteStream.h"


// an alarm count for ms
class Alarm{
    uint64_t time_remain_;
    uint64_t total_time_;
    bool iscounting_;
    bool istimeout_;
public:
    Alarm(uint64_t default_timeout){
        total_time_ = default_timeout;
        iscounting_ = false;
        istimeout_  = false;
    }
    void StartResetAlarm(){
        iscounting_ = true;
        time_remain_= total_time_;
        istimeout_  = false;
    }
    void EndAlarm(){
        iscounting_ = false;
        time_remain_ = total_time_;
        istimeout_  = false;
    }
    void TimeElasp(uint64_t ms){
        if(time_remain_<=ms){
            time_remain_ = 0;
            istimeout_   = true;
        }else
            time_remain_ -= ms;
    }
    bool IsTimeOut(){ return istimeout_;}
    bool IsCounting(){return iscounting_;}

};

using tcallback = std::function<void(int)>;

static void DefaultCallBack(int){}

class GBNSender{
private:
    static const uint16_t DEFAULT_WIN_  = 10;
    static const uint16_t DEFAULT_BUF_  = 30;
    static const uint16_t DEFAULT_TIME_OUT_ = 3000;

    tcallback notify_nwrite_;

    ByteStream sender_stream_;
    std::deque<GBNPDU> sender_queue_;
    Alarm    alarm_;
    uint16_t win_size_;

    uint16_t seq_has_acked_ = 0; // seqnum < this has been acked

    uint16_t next_seq_ = 0; // seqnum < this has been sent
public:
    GBNSender(
            uint16_t win_size = DEFAULT_WIN_,
            uint16_t time_out = DEFAULT_TIME_OUT_
    ): win_size_(win_size), alarm_(time_out){
        if(win_size==0)
            throw std::runtime_error("GBNSender cannot use zero window");
        notify_nwrite_ = DefaultCallBack;
    }
    void SendAck(uint16_t ackno);

    void TimeElasped(uint64_t ms_ticked);

    // properly set the seq and so on
    /*
     * called when:
     *  ack receive: update the window and send more pkg;
     *  user write:  user write to the socket, then try its best to fill up window;
     */
    void FillWindow();
    void AckReceived(uint16_t ackno);

    void RegisterCallBack(tcallback tcb){notify_nwrite_ = tcb;}

    bool IsStreamFull(){return sender_stream_.GetFrames().size()>=DEFAULT_BUF_;}

    std::deque<GBNPDU>&GetSenderQueue(){return sender_queue_;};
    ByteStream &GetStream(){return sender_stream_;};
};

#endif //GOBACKN_GBNSENDER_H
