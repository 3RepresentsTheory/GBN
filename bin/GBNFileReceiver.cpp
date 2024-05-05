//
// Created by no3core on 2024/5/2.
//

#include <iostream>
#include <fstream>
#include "../src/GBNSocket.h"

int main(int argc, char* argv[]){
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <output_filename>" << std::endl;
        return 1;
    }

    GBNSocketConfig config("gbnconfig_server.json");
    GBNSocket server(config);

    const std::string outputFilename = argv[1];

    std::ofstream file(outputFilename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to create output file: " << outputFilename << std::endl;
        return 1;
    }

    // Receive and write file content
    char buffer[1024];
    ssize_t bytesRead = 0;
    while ((bytesRead = server.Read(buffer,sizeof(buffer))) > 0) {
        file.write(buffer, bytesRead);
        file.flush();
    }

}