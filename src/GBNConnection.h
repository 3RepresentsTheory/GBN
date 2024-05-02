//
// Created by no3core on 2024/4/26.
//

#ifndef GOBACKN_GBNCONNECTION_H
#define GOBACKN_GBNCONNECTION_H
#include "GBNReceiver.h"
#include "GBNSender.h"
#include "GBNPDU.h"
#include <functional>

using tcallback = std::function<void(int)>;

class GBNConnection{
    friend class GBNSenderTestHarness;
private:
    GBNReceiver receiver_;
public:

    GBNConnection(
            uint16_t win_size,
            uint16_t time_out
    ): sender_(win_size,time_out){};

    GBNConnection(){}

    void RegisterCallBack(tcallback tcb){ sender_.RegisterCallBack(tcb); }

    void PkgReceived(const GBNPDU& moved_pkg);

    void EndInput();
    void TimeElapsed(uint64_t ms);

    size_t Read(char* buf,size_t n);

    size_t Write(const char* buf, size_t n);
    size_t Write(const std::string & str);

    std::deque<GBNPDU> &GetWaitToSent(){return sender_.GetSenderQueue();}
    ByteStream& GetRecvStream(){return receiver_.GetStream();}

    bool IsSenderFull(){return sender_.IsStreamFull();}

    GBNSender   sender_;
};

#endif //GOBACKN_GBNCONNECTION_H
