//
// Created by no3core on 2024/5/1.
//

#include <iostream>
#include "../src/GBNSocket.h"

void EchoServer(uint16_t port) {
    GBNSocket server(port);

    while (true) {
        char buffer[1024];
        size_t bytesRead = server.Read(buffer, sizeof(buffer));
        printf("server gets: %zu bytes: %s\n",bytesRead,buffer);
        if (bytesRead > 0) {
            server.Write(buffer, bytesRead);
        }
    }
}

int main() {
    uint16_t port = 1234;
    EchoServer(port);

    return 0;
}