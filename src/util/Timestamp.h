//
// Created by no3core on 2024/5/5.
//

#ifndef GOBACKN_TIMESTAMP_H
#define GOBACKN_TIMESTAMP_H

std::string formatTime() {
    char buffer[64];
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
    std::strftime(buffer, sizeof(buffer), "%Y/%m/%d %H:%M:%S", std::localtime(&timestamp));
    return std::string(buffer);
}

#endif //GOBACKN_TIMESTAMP_H
