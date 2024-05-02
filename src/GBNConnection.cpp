//
// Created by no3core on 2024/4/26.
//

#include "GBNConnection.h"

void GBNConnection::PkgReceived(const GBNPDU &moved_pkg) {
    //TODO: need check malformed here!!
    if(moved_pkg.malformed_)
        return;

    if(moved_pkg.ackf_)
        sender_.AckReceived(moved_pkg.GetNum());
    else{
        receiver_.PkgReceived(moved_pkg);
        int ackno = receiver_.GetRecvNum()-1;
        if(ackno>=0)
            sender_.SendAck(ackno);
    }
}

void GBNConnection::EndInput() {
    sender_.GetStream().SetEof();
}

size_t GBNConnection::Write(const char *buf, size_t n) {
    auto byte_writes = sender_.GetStream().Write(buf,n);
    sender_.FillWindow();
    return byte_writes;
}

size_t GBNConnection::Write(const std::string &str) {
    auto byte_writes =sender_.GetStream().Write(str);
    sender_.FillWindow();
    return byte_writes;
}

void GBNConnection::TimeElapsed(uint64_t ms) {
    sender_.TimeElasped(ms);
}

size_t GBNConnection::Read(char *buf, size_t n) {
    return receiver_.GetStream().Read(buf,n);
}
