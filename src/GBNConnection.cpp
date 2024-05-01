//
// Created by no3core on 2024/4/26.
//

#include "GBNConnection.h"

void GBNConnection::PkgReceived(const GBNPDU &moved_pkg) {
    if(moved_pkg.ackf_)
        sender_.AckReceived(moved_pkg.GetNum());
    else
        receiver_.PkgReceived(moved_pkg);
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
