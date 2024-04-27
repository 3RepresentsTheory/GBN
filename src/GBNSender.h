//
// Created by no3core on 2024/4/17.
//

#ifndef GOBACKN_GBNSENDER_H
#define GOBACKN_GBNSENDER_H


#include <cstdint>
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
        total_time_ = time_remain_;
        istimeout_  = false;
    }
    void EndAlarm(){
        iscounting_ = false;
        total_time_ = time_remain_;
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

class GBNSender{
private:
    static const uint16_t DEFAULT_WIN_  = 10;
    static const uint16_t DEFAULT_TIME_OUT_ = 500;

    ByteStream sender_stream_;
    Alarm    alarm_;
    uint16_t win_size_;

    uint16_t seq_has_acked_ = 0;
    uint16_t next_seq_ = 0;
public:
    GBNSender(
            uint16_t win_size = DEFAULT_WIN_,
            uint16_t time_out = DEFAULT_TIME_OUT_
    ):win_size_(win_size), alarm_(time_out){}
    void send_ack(uint16_t ackno);

    void time_elasped(uint64_t ms_ticked);

    // properly set the seq and so on
    /*
     * called when:
     *  ack receive: update the window and send more pkg;
     *  user write:  user write to the socket, then try its best to fill up window;
     */
    void fill_window();

    void ack_received(uint16_t ackno);
};

#endif //GOBACKN_GBNSENDER_H
