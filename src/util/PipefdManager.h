//
// Created by no3core on 2024/4/28.
//

#ifndef GOBACKN_PIPEFDMANAGER_H
#define GOBACKN_PIPEFDMANAGER_H
#include <unistd.h>
#include <cstring>
#include <stdexcept>

class PipefdManager{
    // cannot copy the fd
    PipefdManager(const PipefdManager&other) = delete;
    PipefdManager&operator=(const PipefdManager&other) = delete;
    int pipefd[2];
public:
    PipefdManager(){
        if(pipe(pipefd)==-1)
            throw std::runtime_error("cannot create pipe : "+ std::string(std::strerror(errno)));
    }
    int GetReadfd(){ return pipefd[0]; }
    int GetWritefd(){return pipefd[1]; }

};

#endif //GOBACKN_PIPEFDMANAGER_H
