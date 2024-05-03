//
// Created by no3core on 2024/4/17.
//

#include "GBNSender.h"

void GBNSender::SendAck(uint16_t ackno) {
    sender_queue_.emplace_back(GBNPDU(ackno));
    notify_nwrite_(1);
}

void GBNSender::SendFin() {
    sender_stream_.GetFrames().emplace_back(-1,std::string(), true);
    FillWindow();
}

void GBNSender::TimeElasped(uint64_t ms_ticked) {
    if(alarm_.IsCounting()){
        alarm_.TimeElasp(ms_ticked);
        if(alarm_.IsTimeOut()){
            fprintf(stderr,"timeout triggered, retransmit the window\n");
            // get retransmission
            alarm_.StartResetAlarm();
            int i = seq_has_acked_;
            for(auto &pkg: sender_stream_.GetFrames()) {
                if(i==next_seq_)
                    break;
                sender_queue_.push_back(pkg);
                i++;
            }
            notify_nwrite_(next_seq_-seq_has_acked_);
        }
    }
}

void GBNSender::FillWindow() {
    int i = 0;
    int new_num = 0;
    int skip = next_seq_-seq_has_acked_;
    for(auto &pkg: sender_stream_.GetFrames()){
        // window is full ,not fill up window
        if(next_seq_-seq_has_acked_>=win_size_)
            break;

        // fill window at back
        if(i<skip){
            i++;
            continue;
        }

        pkg.ackf_ = false;
        pkg.num_  = next_seq_;

        if(next_seq_==seq_has_acked_)
            // alarm for the first package
            alarm_.StartResetAlarm();

        sender_queue_.push_back(pkg);
        next_seq_++;
        new_num ++;
    }
    notify_nwrite_(new_num);

    // stop timer if no more data to fill up window
    if(
            next_seq_==seq_has_acked_
//            sender_stream_.GetFrames().empty() ??
    )
        alarm_.EndAlarm();
}

void GBNSender::AckReceived(uint16_t ackno) {
    if(ackno>=next_seq_ || ackno<seq_has_acked_)
        // skip the invaild or outdated ackno
        return;

    // drop the pkg that has been acked:
    auto & frames = sender_stream_.GetFrames();
    int seqno;
    while(true){
        seqno = frames.front().num_;
        frames.pop_front();
        if(seqno==ackno) break;
    }

    seq_has_acked_ = ackno+1;
    // restart the timer for seq_has_acked:
    alarm_.StartResetAlarm();
    FillWindow();
}


