//
// Created by no3core on 2024/4/26.
//

#include "GBNByteStream.h"
static const char*n2hex = "0123456789ABCDEF";

size_t ByteStream::Read(char *buffer, size_t n) {
    if(n<0) return -1;

    char* p = buffer;
    while(!pdu_frames_.empty()&&n!=0){

        auto &pkg = pdu_frames_.front();
        fprintf(stderr,
                "picking frame(%p) with PDU data: num %d, len %zu, and data: \n",
                &pkg,
                pkg.GetNum(),
                pkg.GetLen()
        );
        for (int i = 0; i < pkg.GetLen(); ++i) {
            unsigned char x = pkg.GetData()[i];
            fprintf(stderr,"%c%c",n2hex[(x&0xF0)>>4],n2hex[x&0xF]);
            if(i%2==1) fprintf(stderr," ");
        }
        fprintf(stderr,"\n");

        size_t avail_len   = pkg.GetLen()-has_readn_;
        size_t actual_len  = std::min(avail_len,n);
        pkg.GetData().copy(p,actual_len,has_readn_);

        if(n>=avail_len){
            pdu_frames_.pop_front();
            has_readn_ = 0;
        }else{
            has_readn_+=n;
        }
        p+=actual_len;
        n-=actual_len;
    }
    return p-buffer;
}

// simply for TEST use, for the heavy copy here
std::string ByteStream::Read(size_t n) {
    char*p = new char [n];
    size_t actual_readn;
    if((actual_readn=Read(p,n))==-1){
        throw std::runtime_error("Read error, cannot construct a string");
    }
    std::string ret_string(p,actual_readn);
    delete[] p;
    return ret_string;
}


size_t ByteStream::Write(const char *buffer, size_t n) {
    if(IsEofed())
        throw std::runtime_error("write to an ended pipe");

    size_t largest_pkg_size = GBNPDU::LARGEST_DATA_SIZE;
    const char *p = buffer;
    while(n){
        size_t availabel_size = std::min(n,largest_pkg_size);

        // TODO: pretty strange here, if I initialize it with zero, there may be some zero tagged
        // pdu in the sender queue
        pdu_frames_.emplace_back(-1,std::string(p,availabel_size), false);
//        fprintf(stderr,"pushing a frame(%p) size at %p %zu with : %s\n",&pdu_frames_.back(),p,availabel_size,pdu_frames_.back().GetData().c_str());
        p+=availabel_size;
        n-=availabel_size;
    }
    return p-buffer;
}

// simply for TEST use, string may bind to "" , please specify len here
size_t ByteStream::Write(const std::string &string) {
    return Write(string.c_str(), string.length());
}

void ByteStream::PushPDU(const GBNPDU &moved_pdu) {
    pdu_frames_.emplace_back(std::move(moved_pdu));
}

