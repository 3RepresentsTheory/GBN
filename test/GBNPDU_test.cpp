//
// Created by no3core on 2024/4/17.
//


#include <string>
#include <iostream>
#include <cassert>
#include "../src/GBNPDU.h"

static void insert_u16_b(uint16_t n,std::string&frame){
    frame.push_back((n>>8)&0xFF);
    frame.push_back(n&0xFF);
}

int main(){

    setbuf(stdout,0);
    // check ccitt checksum:
    std::string test_string1 = "01";
    auto chksum = GBNPDU::CalCheckSum(test_string1);
    insert_u16_b(chksum,test_string1);
    assert(GBNPDU::CheckSumCal(test_string1));



    // generate data package
    std::string data1 = "1234";
    GBNPDU PkgGenTest(0x1234,data1);
    auto PkgGenTest_frame1 = PkgGenTest.Serialize();
    GBNPDU::HexDump(PkgGenTest_frame1);

    // generate ack package
    GBNPDU AckGenTest(0x1145);
    auto AckGenTest_frame1 = AckGenTest.Serialize();
    GBNPDU::HexDump(AckGenTest_frame1);

    // deserialize package above
    GBNPDU PkgGenTestDeserialize(std::move(PkgGenTest_frame1));
    auto PkgGenTest_frame2 = PkgGenTestDeserialize.Serialize();
    GBNPDU::HexDump(PkgGenTest_frame2);
    assert(PkgGenTestDeserialize.IsFormated());

    GBNPDU AckGenTestDeseralize(std::move(AckGenTest_frame1));
    auto AckGenTest_frame2 = AckGenTestDeseralize.Serialize();
    GBNPDU::HexDump(AckGenTest_frame2);
    assert(AckGenTestDeseralize.IsFormated());

    // generate data package 2
    std::string data2 = "";
    GBNPDU PkgGenTest2(-1,data2);
    auto PkgGenTest_frame3 = PkgGenTest2.Serialize();
    GBNPDU::HexDump(PkgGenTest_frame3);
}
