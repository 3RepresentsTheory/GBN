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
        buffer[bytesRead]='\0';
        fprintf(stderr,"server gets: %zu bytes to(%p) %s\n",bytesRead,buffer,buffer);
        if (bytesRead > 0) {
            auto bytesWrite = server.Write(buffer, bytesRead);
            fprintf(stderr,"server writes: %zu bytes\n",bytesWrite);
        }
    }
}

int main() {
    setbuf(stdout,0);
    uint16_t port = 1234;
    EchoServer(port);

    return 0;
}