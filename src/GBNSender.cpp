//
// Created by no3core on 2024/4/17.
//

#include "GBNSender.h"

void GBNSender::send_ack(uint16_t ackno) {

}

void GBNSender::time_elasped(uint64_t ms_ticked) {
    if(alarm_.IsCounting()){
        alarm_.TimeElasp(ms_ticked);
        if(alarm_.IsTimeOut()){
            // get retransmission
            alarm_.StartResetAlarm();
            int i = seq_has_acked_ + 1;
            for(auto &pkg: sender_stream_.GetFrames()) {
                if(i!=next_seq_){
                    // send pkg to the epoll
                    //...
                    //
                }
            }
        }
    }
}

void GBNSender::fill_window() {
    for(auto &pkg: sender_stream_.GetFrames()){
        if(next_seq_-seq_has_acked_>=win_size_)
            break;
        pkg.ackf_ = false;
        pkg.num_  = next_seq_;

        if(next_seq_==seq_has_acked_+1)
            // alarm for the first package
            alarm_.StartResetAlarm();

        // send pkg to the epoll
        //...
        //
        next_seq_++;
    }
}

void GBNSender::ack_received(uint16_t ackno) {
    if(ackno>next_seq_ || ackno)
        // skip the invaild or outdated ackno
        return;

    // drop the pkg that has been acked:
    auto & frames = sender_stream_.GetFrames();
    int seqno= frames.front().num_;
    while(seqno<=ackno){
        frames.pop_front();
        seqno = frames.front().num_;
    }

    seq_has_acked_ = ackno;
    fill_window();
}

