//
// Created by no3core on 2024/5/2.
//
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../src/GBNSocket.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <local_port> <peer_port> <peer_ip> <filename>" << std::endl;
        return 1;
    }

//    uint16_t local_port = std::stoi(argv[1]);
//    uint16_t peer_port = std::stoi(argv[2]);
//    const std::string peer_ip = argv[3];
    uint16_t local_port = 5678;
    uint16_t peer_port  = 1234;
    const std::string filename = argv[1];

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return 1;
    }

    GBNSocket sender(local_port,peer_port,"127.0.0.1");

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