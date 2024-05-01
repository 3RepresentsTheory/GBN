//
// Created by no3core on 2024/4/26.
//

#ifndef GOBACKN_GBNBYTESTREAM_H
#define GOBACKN_GBNBYTESTREAM_H

#include <cstdio>
#include <deque>
#include <stdexcept>

#include "GBNPDU.h"

class ByteStream{
private:
    std::deque<GBNPDU> pdu_frames_;
    size_t has_readn_;
    bool   iseof_;
public:
    ByteStream(){
        has_readn_ =0;
        iseof_ = false;
    }
    std::string Read(size_t n);
    size_t Read(char* buffer,size_t n);
    size_t Write(const char*buffer,size_t n);
    size_t Write(const std::string& string);
    void   PushPDU(const GBNPDU& moved_pdu);
    void   SetEof(){iseof_ = true;}

    bool   IsEofed(){return iseof_;}
    bool   IsEnded(){return iseof_ && pdu_frames_.empty();}
    bool   IsEmpty(){return pdu_frames_.empty();}

    // return the pdu_frames for retransmission
    std::deque<GBNPDU>&GetFrames(){return pdu_frames_;};
};



#endif //GOBACKN_GBNBYTESTREAM_H
