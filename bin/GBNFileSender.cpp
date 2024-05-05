//
// Created by no3core on 2024/5/2.
//
#include <iostream>
#include <fstream>
#include <string>
#include "../src/GBNSocket.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <local_port> <peer_port> <peer_ip> <filename>" << std::endl;
        return 1;
    }

    GBNSocketConfig config("gbnconfig_client.json");
    const std::string filename = argv[1];

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return 1;
    }

    GBNSocket sender(config,51224,"127.0.0.1");

    // 发送文件内容
    char buffer[1024];
    while (!file.eof()) {
        file.read(buffer, sizeof(buffer));
        size_t bytesRead = file.gcount();
        sender.Write(buffer,bytesRead);
    }

    file.close();
    return 0;
}