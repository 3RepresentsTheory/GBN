//
// Created by no3core on 2024/5/1.
//

#include <iostream>
#include "../src/GBNSocket.h"

void EchoClient(uint16_t localPort, uint16_t peerPort, const std::string& peerIP) {
    GBNSocket client(localPort, peerPort, peerIP);

    std::string message;
    while (std::getline(std::cin, message)) {
        client.Write(message);
        auto response = client.Read(message.size());  // 从服务器读取响应
        std::cout << "Server response: " << response << std::endl;
    }
}

int main() {
    uint16_t localPort = 5678;
    uint16_t peerPort = 1234;
    std::string peerIP = "127.0.0.1";

    EchoClient(localPort, peerPort, peerIP);

    return 0;
}
