//
// Created by no3core on 2024/4/26.
//

#include <cstring>
#include <cassert>
#include <iostream>
#include "../src/GBNByteStream.h"
using ccstring = const char[];

int main(){
    {
        // basic read write test
        ByteStream test;
        ccstring test_string1 = "0123456789ABCDEF"
                                "0123456789ABCDEF"
                                "0123456789ABCDEF"
                                "0123456789ABCDEF"
                                "0123456789ABCDEF"
                                "0123456";
        test.Write(test_string1,sizeof(test_string1));

//        for(auto &x:test.GetFrames()){
//            GBNPDU::HexDump(x.Serialize());
//        }

        char test_string1_back[sizeof(test_string1)];
        test.Read(test_string1_back,sizeof(test_string1));
        assert(strcmp(test_string1_back,test_string1)==0);
    }

    {
        // write empty string
        ByteStream test;
        test.Write("");
        assert(test.GetFrames().empty());
        assert(test.Read(100)=="");

    }
    {
        // write null string
        ByteStream test;
        std::string null_string = std::string(10,'\0');
        test.Write(null_string);
        assert(test.Read(1000)==null_string);
    }

    {
        // multiple read write test
        ByteStream test;
        test.Write("0123456789");
        test.Write("0123456789876543210");

        assert(test.Read(5)=="01234");
        assert(test.Read(999)=="567890123456789876543210");
        assert(test.GetFrames().empty());
        test.Read(10);
        assert(test.GetFrames().empty());

        test.Write("abc");
        assert(test.Read(100)=="abc");
    }

}