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
    uint16_t expected_seq_ ;
public:
    // make sure only data pkg move to here
    GBNReceiver(uint16_t initial_seqno = 0):expected_seq_(initial_seqno){};

    void PkgReceived(const GBNPDU&moved_pdu){
        if(receiver_stream_.IsEofed()) return;
        if(moved_pdu.GetNum() == expected_seq_){
            expected_seq_++;
            // only push to stream when the pkg contains data
            if(moved_pdu.GetLen()!=0)
                receiver_stream_.PushPDU(moved_pdu);
            if(moved_pdu.Fin()) receiver_stream_.SetEof();
        }
    }
    ByteStream& GetStream(){return receiver_stream_;}


    // notice that seq_ indicating the next expected seq, a
    // ckno == expected_seq_-1
    uint16_t GetRecvNum(){return expected_seq_;}
};

#endif //GOBACKN_GBNRECEIVER_H
