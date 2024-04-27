//
// Created by no3core on 2024/4/27.
//


#include <cassert>
#include "../src/GBNReceiver.h"

int main(){
    {
        // normal test
        GBNReceiver test;

        test.PkgReceived(GBNPDU(0,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(1,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(2,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(3,"0123456789ABCDEF", false));

        assert(test.GetRecvNum()==4);
        assert(test.GetStream().Read(100)=="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF");
        assert(test.GetStream().GetFrames().empty());
    }

    {
        // ignore the seqno not matched
        GBNReceiver test;

        test.PkgReceived(GBNPDU(1,"0123456789ABCDEF", false));
        assert(test.GetRecvNum()==0);
        assert(test.GetStream().GetFrames().empty());

        test.PkgReceived(GBNPDU(0,"jiwangzhentule", false));
        assert(test.GetRecvNum()==1);
        assert(!test.GetStream().GetFrames().empty());
        test.PkgReceived(GBNPDU(2,"jiwangzhentule", false));
        test.PkgReceived(GBNPDU(1,"jiwangzhentule", false));
        assert(test.GetRecvNum()==2);
        assert(test.GetStream().Read(1000)=="jiwangzhentulejiwangzhentule");
    }
}
