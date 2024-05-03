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
    {
        // passing empty package with seqno
        GBNReceiver test;
        test.PkgReceived(GBNPDU(0,"", false));
        test.PkgReceived(GBNPDU(1,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(2,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(3,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(4,"", false));
        test.PkgReceived(GBNPDU(5,"", false));
        test.PkgReceived(GBNPDU(6,"", false));
        test.PkgReceived(GBNPDU(7,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(8,"", false));
        test.PkgReceived(GBNPDU(9,"", false));


        assert(test.GetRecvNum()==10);
        assert(test.GetStream().Read(100)=="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF");
        assert(test.GetStream().GetFrames().empty());
    }

    {
        // fin test 1: normal fin test
        GBNReceiver test;
        test.PkgReceived(GBNPDU(0,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(1,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(2,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(3,"0123456789ABCDEF", false));

        assert(test.GetRecvNum()==4);
        test.PkgReceived(GBNPDU(4,"", true));
        // the receiver is not ended due to data is still in the buffer
        assert(!test.GetStream().IsEnded());
        assert(test.GetStream().IsEofed());

        // not all consumed
        assert(test.GetStream().Read(21)=="0123456789ABCDEF01234");
        assert(!test.GetStream().IsEnded());
        assert(test.GetStream().IsEofed());

        assert(test.GetStream().Read(100)=="56789ABCDEF0123456789ABCDEF0123456789ABCDEF");
        assert(test.GetStream().IsEnded());
    }

    {
        // fin test 2: directly get fin
        GBNReceiver test;
        test.PkgReceived(GBNPDU(0,"", true));
        assert(test.GetRecvNum()==1);
        assert(test.GetStream().IsEnded());
    }

    {
        // fin test 3: ignore the package when has been eofed
        GBNReceiver test;
        test.PkgReceived(GBNPDU(0,"", false));
        test.PkgReceived(GBNPDU(1,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(2,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(3,"0123456789ABCDEF", false));
        test.PkgReceived(GBNPDU(4,"0123456789ABCDEF", false));
        // eof
        test.PkgReceived(GBNPDU(5,"", true));

        test.PkgReceived(GBNPDU(6,"cannot be get", false));
        test.PkgReceived(GBNPDU(6,"cannot be get", true));

        assert(test.GetRecvNum()==6);
        assert(test.GetStream().IsEofed());
        assert(!test.GetStream().IsEnded());
        assert(test.GetStream().Read(100)=="0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF");
        assert(test.GetStream().IsEofed());
        assert(test.GetStream().IsEnded());
    }
}
