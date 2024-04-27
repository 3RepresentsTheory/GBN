//
// Created by no3core on 2024/4/27.
//

#include "../src/GBNSender.h"

#ifndef GOBACKN_GBNSENDERTESTHARNESS_H
#define GOBACKN_GBNSENDERTESTHARNESS_H

class TestException:public std::exception{
private:
    std::string msg_;
public:
    TestException(const std::string& msg):msg_(msg){}
    virtual const char *what() const noexcept override{
        return msg_.c_str();
    }
};

class GBNSenderTestHarness{
    GBNSender & gbnSender_;
    GBNPDU temp_pdu_;
public:
    GBNSenderTestHarness(GBNSender&gbnSender):
        gbnSender_(gbnSender),
        temp_pdu_(GBNPDU(0)){}

    void ExpectNoPDU();
    GBNSenderTestHarness& ExpectPDU();

    GBNSenderTestHarness& Fin();
    GBNSenderTestHarness& Num(uint16_t num);
    GBNSenderTestHarness& Data(const std::string& str);


};

#endif //GOBACKN_GBNSENDERTESTHARNESS_H
