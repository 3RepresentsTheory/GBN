//
// Created by no3core on 2024/4/17.
//

#ifndef GOBACKN_GBNPDU_H
#define GOBACKN_GBNPDU_H

#include <cstdint>
#include <string>


/*
 * PDU format :
 * |   0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f|
 * |-------------------------framehead------------------------------|
 * |         GOBACK TO N(magic)    |  af(1)|ackno/seqno(2)|length(2)|
 * |-----------------------framebody-start--------------------------|
 * |                                                                |
 * |                  DATA_PART(align to 16 bytes)                  |
 * |                                                                |
 * |----------------------framebody-end-----------------------------|
 * |                   checksum(big-endian)                         |
 *
 */



// magic number for checking
static const char MAGIC_NUM[] = "GOBACK TO N";

class GBNPDU{
    static const unsigned MAX_PKG_SIZE_ = 65507;
    using uint16 = uint16_t;
    using uint8  = uint8_t;
    bool malformed_ = false;


    // frame head
    uint16 checksum_;   // calc checksum when serialize
    uint16 length_;
    // when ackf is true, num_ as ackno, otherwise seqno
    bool ackf_;
    uint16 num_;

    // frame body
    std::string data_;


    // TODO: udp max package size and alignment
    constexpr static size_t LARGEST_DATA_SIZE =
            (MAX_PKG_SIZE_-16-sizeof(num_)-sizeof(length_));
public:

    // generate ack  package
    GBNPDU(uint16 acknum);
    // generate data package
    GBNPDU(uint16 seqnum,const std::string&data);
    // Deserialize construct
    GBNPDU(std::string &&frame);

    std::string Serialize();
    void Deserialize(std::string& bytestream);

    bool IsFormated(){return !malformed_;};


    static bool   CheckSumCal(const std::string& data);
    static uint16 CalCheckSum(const std::string& data);
    static void HexDump(const std::string&bytestream);
};




#endif //GOBACKN_GBNPDU_H
