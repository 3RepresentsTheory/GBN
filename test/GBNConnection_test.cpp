//
// Created by no3core on 2024/4/28.
//


#include <cassert>
#include "../src/GBNConnection.h"
#include "GBNSenderTestHarness.h"

int main(){
    {

        // work in pdu data part 16 bytes:

        GBNConnection test(5,100);
        GBNSenderTestHarness harness(test.sender_);

        test.Write(
                "0123456789ABCDEF"
                "0123456789ABCDEF"
                "0123456789ABCDEF"
                "0123456789ABCDEF"
                "0123456789ABCDEF"
        );

        harness.ExpectPDU().Num(0).Data("0123456789ABCDEF");
        harness.ExpectPDU().Num(1).Data("0123456789ABCDEF");
        harness.ExpectPDU().Num(2).Data("0123456789ABCDEF");
        harness.ExpectPDU().Num(3).Data("0123456789ABCDEF");
        harness.ExpectPDU().Num(4).Data("0123456789ABCDEF");

        test.TimeElapsed(101);

        harness.ExpectPDU().Num(0).Data("0123456789ABCDEF");
        harness.ExpectPDU().Num(1).Data("0123456789ABCDEF");
        harness.ExpectPDU().Num(2).Data("0123456789ABCDEF");
        harness.ExpectPDU().Num(3).Data("0123456789ABCDEF");
        harness.ExpectPDU().Num(4).Data("0123456789ABCDEF");

        test.PkgReceived(GBNPDU(3));
        // should drop it
        test.PkgReceived(GBNPDU(1,"teststring1",false));
        test.PkgReceived(GBNPDU(2,"teststring2",false));
        test.PkgReceived(GBNPDU(6));

        assert(test.GetRecvStream().GetFrames().empty());
        test.TimeElapsed(101);
        harness.ExpectPDU().Num(4).Data("0123456789ABCDEF");
        harness.ExpectNoPDU();



    }
}
