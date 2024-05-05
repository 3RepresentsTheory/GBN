//
// Created by no3core on 2024/4/17.
//

#include <iostream>
#include <cstring>
#include "GBNPDU.h"


std::map<int,std::string> GBNPDU::statemap_ = {
        {0,"New"},
        {1,"TO"}, // Timeout retransmission
};


size_t GBNPDU::LARGEST_DATA_SIZE = (MAX_PKG_SIZE_-16-2);


static void insert_u16(uint16_t n,std::string&frame){
    frame.push_back(n&0xFF);
    frame.push_back((n>>8)&0xFF);
}
// big endian ver
static void insert_u16_b(uint16_t n,std::string&frame){
    frame.push_back((n>>8)&0xFF);
    frame.push_back(n&0xFF);
}
static void insert_u8(uint8_t n,std::string&frame){
    frame.push_back(n&0xFF);
}
static uint16_t retrieve_u16(const std::string&frame){
    uint16_t ret = 0;
    if(frame.size()>2){
        ret += (uint8_t)frame[0];
        ret += ((uint8_t)frame[1])<<8;
    }
    return ret;
}

static uint16_t retrieve_u16(const char*buf){
    uint16_t ret = 0;
    ret += (uint8_t)buf[0];
    ret += ((uint8_t)buf[1])<<8;
    return ret;
}

static uint8_t retrieve_u8(const std::string&frame){
    uint8_t ret = 0;
    if(frame.size()>1){
        ret += (uint8_t)frame[0];
    }
    return ret;
}

static uint8_t retrieve_u8(const char*buf){
    uint8_t ret = 0;
    ret += (uint8_t)buf[0];
    return ret;
}

// make all little endian
std::string GBNPDU::Serialize() const{
    std::string pdu_frame;

    pdu_frame.append(MAGIC_NUM);
    insert_u8((uint8)finf_,pdu_frame);
    insert_u8((uint8)ackf_,pdu_frame);
    insert_u16(num_,   pdu_frame);
    insert_u16(length_,pdu_frame);

    pdu_frame.append(data_);
    size_t paddings = (16-length_%16)%16;
    for (int i = 0; i < paddings; ++i) {
        pdu_frame.push_back(0);
    }

    auto cksum = CalCheckSum(pdu_frame);
    insert_u16_b(cksum,pdu_frame);

    return pdu_frame;
}


// TODO: heavy copy here , need reduce of copyng data_
void GBNPDU::Deserialize(std::string &bytestream) {
    if(
        !CheckSumCal(bytestream) ||
        bytestream.substr(0,sizeof(MAGIC_NUM)-1)!=MAGIC_NUM
    ){
        malformed_ = true;
        return;
    }

    // TODO: reduce copy here
    bytestream.erase(0,sizeof(MAGIC_NUM)-1);
    finf_   = retrieve_u8(bytestream);
    bytestream.erase(0,1);
    ackf_   = retrieve_u8(bytestream);
    bytestream.erase(0,1);
    num_    = retrieve_u16(bytestream);
    bytestream.erase(0,2);
    length_ = retrieve_u16(bytestream);
    bytestream.erase(0,2);
    data_   = bytestream.substr(0,length_);
    malformed_ = false;
}

void GBNPDU::Deserialize(const char *buf,size_t n) {
    if(
        !CheckSumCal(buf,n)||
        strncmp(buf,MAGIC_NUM,sizeof(MAGIC_NUM)-1)
    ){
        malformed_ = true;
        return;
    }
    char *p = const_cast<char *>(buf);
    p+=sizeof(MAGIC_NUM)-1;
    finf_ = retrieve_u8(p);
    p+=1;
    ackf_ = retrieve_u8(p);
    p+=1;
    num_  = retrieve_u16(p);
    p+=2;
    length_= retrieve_u16(p);
    p+=2;
    data_ = std::string(p,length_);
    malformed_ = false;
}



GBNPDU::GBNPDU(GBNPDU::uint16 acknum) {
    state_  = NOTDATA;

    length_ = 0;
    ackf_   = true;
    num_    = acknum;
    checksum_ = 0;
    malformed_ = false;
}

GBNPDU::GBNPDU(GBNPDU::uint16 seqnum, const std::string&data, bool fin = false) {
    if(data.length()>LARGEST_DATA_SIZE){
        malformed_ = true;
        throw std::runtime_error("GBNPDU data is too large");
    }
    state_  = NEW;

    data_   = data;
    finf_   = fin;
    length_ = data_.length();
    num_    = seqnum;
    ackf_   = false;
    checksum_ = 0;
    malformed_ = false;
}


GBNPDU::GBNPDU(std::string &&frame) {
    state_ = NEW;
    Deserialize(frame);
}

GBNPDU::GBNPDU(const char *buf,size_t n) {
    state_ = NEW;
    Deserialize(buf,n);
}



static inline uint16_t crc16_ccitt_init(void) { return 0xFFFF; }
static inline uint16_t crc16_ccitt_update(uint8_t byte, uint16_t crc) {
    int i;
    int xor_flag;

    /* For each bit in the data byte, starting from the leftmost bit */
    for (i = 7; i >= 0; i--) {
        /* If leftmost bit of the CRC is 1, we will XOR with
         * the polynomial later */
        xor_flag = crc & 0x8000;
        /* Shift the CRC, and append the next bit of the
         * message to the rightmost side of the CRC */
        crc <<= 1;
        crc |= (byte & (1 << i)) ? 1 : 0;
        /* Perform the XOR with the polynomial */
        if (xor_flag)
            crc ^= 0x1021;
    }

    return crc;
}
static inline uint16_t crc16_ccitt_finalize(uint16_t crc) {
    int i;
    /* Augment 16 zero-bits */
    for (i = 0; i < 2; i++) {
        crc = crc16_ccitt_update(0, crc);
    }
    return crc;
}


// crc16_ccitt cheksum
GBNPDU::uint16 GBNPDU::CalCheckSum(const std::string& data) {
    uint16 crc = crc16_ccitt_init();
    for(auto const &x:data){
        crc = crc16_ccitt_update(x,crc);
    }
    return crc16_ccitt_finalize(crc);
}

bool GBNPDU::CheckSumCal(const std::string& data) {
    uint16 crc = crc16_ccitt_init();
    for(auto const &x:data){
        crc = crc16_ccitt_update(x,crc);
    }
    return crc==0;
}

bool GBNPDU::CheckSumCal(const char *buf, size_t n) {
    uint16 crc = crc16_ccitt_init();
    for(int i = 0;i<n;i++){
        crc = crc16_ccitt_update(buf[i],crc);
    }
    return crc==0;
}

void GBNPDU::HexDump(const std::string &bytestream) {
    static const char* hex = "0123456789ABCDEF";
    int i = 0;
    printf("hexdump of string:\n");
    for(const auto &x:bytestream){
        printf("%c%c",hex[(x>>4)&0xF],hex[x&0xF]);
        i++;
        if(i%4==0) printf(" ");
        if(i%16==0) printf("\n");
    }
    if(i%16!=0) printf("\n");
}






