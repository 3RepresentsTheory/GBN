//
// Created by no3core on 2024/4/27.
//

#include "GBNSenderTestHarness.h"

void GBNSenderTestHarness::ExpectNoPDU() {
    if(!gbnSender_.GetSenderQueue().empty())
        throw TestException("Expect: no pdu should been sent");
}

GBNSenderTestHarness &GBNSenderTestHarness::ExpectPDU() {
    if(gbnSender_.GetSenderQueue().empty())
        throw TestException("Expect: should have pdu");

    // move the sender queue to temp
    temp_pdu_ = std::move(gbnSender_.GetSenderQueue().front());
    gbnSender_.GetSenderQueue().pop_front();

    return *this;
}

GBNSenderTestHarness &GBNSenderTestHarness::Fin() {
    if(!temp_pdu_.Fin())
        throw TestException("Expect: pdu need fin flag");
    return *this;
}

GBNSenderTestHarness &GBNSenderTestHarness::Num(uint16_t num) {
    if(temp_pdu_.GetNum()!=num)
        throw TestException(
                "Expect: pdu num should be "+ std::to_string(num)
                +" but get " + std::to_string(temp_pdu_.GetNum()));
    return *this;
}

GBNSenderTestHarness &GBNSenderTestHarness::Data(const std::string &str) {
    if(temp_pdu_.GetData()!=str)
        throw TestException(
            "Expect: pdu str should be " + str
            +" but get " + temp_pdu_.GetData()
        );
    return *this;
}
