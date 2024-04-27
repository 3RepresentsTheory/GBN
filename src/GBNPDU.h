//
// Created by no3core on 2024/4/17.
//

#ifndef GOBACKN_GBNPDU_H
#define GOBACKN_GBNPDU_H

#include <cstdint>
#include <string>
#include <stdexcept>


/*
 * PDU format :
 * |   0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f|
 * |-------------------------framehead------------------------------|
 * |         GOBACKTON1(magic) |fi(1)|af(1)|ackno/seqno(2)|length(2)|
 * |-----------------------framebody-start--------------------------|
 * |                                                                |
 * |                        DATA_PART(4KB)                          |
 * |                                                                |
 * |----------------------framebody-end-----------------------------|
 * |                   checksum(big-endian)                         |
 *
 */



// magic number for checking
static const char MAGIC_NUM[] = "GOBACKTON1";

class GBNPDU{
    friend class GBNByteStream;
    friend class GBNSender;

//    static const unsigned MAX_PKG_SIZE_ = 4096;
    static const unsigned MAX_PKG_SIZE_ = 16+2+16;
    using uint16 = uint16_t;
    using uint8  = uint8_t;
    bool malformed_ = false;


    // frame head
    uint16 checksum_;   // set when serialize

    uint16 length_;     // set when write to bytestream
    // when ackf is true, num_ as ackno, otherwise seqno
    bool ackf_;
    bool finf_;         // set when write to bytestream
    uint16 num_;        // set when fill window

    // frame body
    std::string data_;  // set when write to bytestream


public:

    // TODO: udp max package size and alignment
    constexpr static size_t LARGEST_DATA_SIZE =
            (MAX_PKG_SIZE_-16-2);

    // generate ack  package
    GBNPDU(uint16 acknum);
    // generate data package
    GBNPDU(uint16 seqnum,const std::string&data,bool fin);
    // Deserialize construct
    GBNPDU(std::string &&frame);

    std::string Serialize ()const;
    void Deserialize(std::string& bytestream);

    bool IsFormated(){return !malformed_;};


    static bool   CheckSumCal(const std::string& data);
    static uint16 CalCheckSum(const std::string& data);
    static void HexDump(const std::string&bytestream);


    size_t GetLen() const{ return length_;}
    std::string GetData() const{return data_;}
    uint16 GetNum() const{return num_;}
    bool   Fin() const{return finf_;};
};




#endif //GOBACKN_GBNPDU_H
