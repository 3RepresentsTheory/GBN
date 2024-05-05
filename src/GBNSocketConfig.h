//
// Created by no3core on 2024/5/4.
//

#ifndef GOBACKN_GBNSOCKETCONFIG_H
#define GOBACKN_GBNSOCKETCONFIG_H


#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <string>

using json = nlohmann::json;

class GBNSocketConfig{
public:
    uint16_t timeout_;
    uint16_t initseqno_;
    uint16_t swsize_;
    uint16_t lostrate_;
    uint16_t errorrate_;
    size_t   datasize_;
    uint16_t localport_;

    GBNSocketConfig(const std::string &path){
        std::ifstream confile(path);
        std::stringstream buffer;
        if(confile){
            buffer<< confile.rdbuf();
            confile.close();
            json data = json::parse(buffer.str());

            if(data["UDPPort"].is_null()) goto error;
            localport_ = data["UDPPort"];
            if(data["DataSize"].is_null()) goto error;
            datasize_  = data["DataSize"];
            if(data["ErrorRate"].is_null()) goto error;
            errorrate_ = data["ErrorRate"];
            if(data["LostRate"].is_null()) goto error;
            lostrate_  = data["LostRate"];
            if(data["SWSize"].is_null()) goto error;
            swsize_    = data["SWSize"];
            if(data["InitSeqNo"].is_null()) goto error;
            initseqno_ = data["InitSeqNo"];
            if(data["Timeout"].is_null()) goto error;
            timeout_   = data["Timeout"];

            return;

            error:
            throw std::runtime_error("config file mal formated");
        }else{
            throw std::runtime_error("failed to open the config file");
        }
    }

};

#endif //GOBACKN_GBNSOCKETCONFIG_H
