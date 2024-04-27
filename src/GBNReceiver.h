//
// Created by no3core on 2024/4/17.
//

#ifndef GOBACKN_GBNRECEIVER_H
#define GOBACKN_GBNRECEIVER_H


#include "GBNPDU.h"
#include "GBNByteStream.h"
#include <cstdint>

/*
 * GBN Receiver state machine:
 *
 * initial state:
 *      wait ack for 0
 *
 * when receive an data package
 *      if in initial state and not match:
 *          ignore
 *      if match the recv seq:
 *          pass the PDU data part to the bytestream handle by socket
 *          update the recv-seq
 *      report the current recv seq
 *
 */


class GBNReceiver{
    ByteStream receiver_stream_;
    uint16_t seq_ = 0;
public:
    void PkgReceived(GBNPDU&moved_pdu){
        if(moved_pdu.GetNum()==seq_){
            seq_++;
            receiver_stream_.PushPDU(moved_pdu);
            if(moved_pdu.Fin()) receiver_stream_.SetEof();
        }
    }
    ByteStream& GetStream(){return receiver_stream_;}
};

#endif //GOBACKN_GBNRECEIVER_H
