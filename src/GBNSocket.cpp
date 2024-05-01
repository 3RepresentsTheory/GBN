//
// Created by no3core on 2024/4/17.
//

#include <iostream>
#include "GBNSocket.h"




GBNSocket::GBNSocket(
        uint16_t local_port
): sfd_(local_port)
{
    /*
     * here the socket only support one connect, so peer addr and port:
     * when the socket create, it is listening
     *      peer address and port will be set when pkg arrived
     * when the socket sent package actively, use SetAddress to sent
     */

    local_port_= local_port;

    // event register:

    // basic listening event
    int sockfd = sfd_.GetFd();
    eventloop_.RegisterEvent(
        sockfd,
        EPOLLIN,
        [sockfd,this](){ FrameReceived(sockfd); }
    );

    // time trigger event
    int timerfd= tfd_.GetFd();
    uint64_t interval = tfd_.GetInterval();
    eventloop_.RegisterEvent(
        timerfd,
        EPOLLIN|EPOLLET,
        [timerfd,interval,this](){
            uint64_t expiration = 0;
            read(timerfd,&expiration,sizeof(expiration));
            this->connection_.TimeElapsed(expiration*interval);
        }
    );

    // write event
    int pipefd_read = pfd_.GetReadfd();
    int pipefd_write= pfd_.GetWritefd();
    connection_.RegisterCallBack(
        [pipefd_write](int num){ write(pipefd_write,&num,sizeof(num)); }
    );
    eventloop_.RegisterEvent(
        pipefd_read,
        EPOLLIN,
        [pipefd_read,this](){ FrameSent(pipefd_read);}
    );


    // launch the thread for event loop:
    eventthread_ = std::thread(&EventLoop::Loop,&eventloop_);

}

GBNSocket::~GBNSocket() {
    eventloop_.StopLoop();
    eventthread_.join();
}

constexpr int BUFFER_SIZE = 65507 ; // max udp package
void GBNSocket::FrameReceived(int socketfd) {
    std::unique_lock<std::mutex> lock(read_mtx_);

    char buffer[BUFFER_SIZE];
    sockaddr_in clientAddr{};
    socklen_t   addrLen = sizeof(clientAddr);
    ssize_t bytesRead =
            recvfrom(
                socketfd,
                buffer,
                BUFFER_SIZE,
                0,
                reinterpret_cast<sockaddr*>(&clientAddr),
                &addrLen
            );

    auto && temp_pkg =GBNPDU(buffer,bytesRead);
    if(!temp_pkg.IsFormated()){
        std::cout << "detect unformated package" << std::endl;
        return;
    }
    connection_.PkgReceived(temp_pkg);

    // notify the read is 'perhaps' available now, for it might be ack package
    // (not nice design here)
    read_cv_.notify_one();

    if(peer_addr_.sin_port==0)
        peer_addr_ = clientAddr;
}

void GBNSocket::FrameSent(int eventfd) {
    if(peer_addr_.sin_port==0)
        throw std::runtime_error("GBNSocket: set peer address first!");
    int sentnum;
    // temp no checking
    read(eventfd,&sentnum,sizeof(sentnum));

    int sockfd = sfd_.GetFd();
    auto & sentq = connection_.GetWaitToSent();
    for (int i = 0; i < sentnum && !sentq.empty(); ++i) {
        auto &pkg = sentq.front();
        auto &&serial = pkg.Serialize();
        ssize_t bytesSent = sendto(
                sockfd,
                serial.c_str(),
                serial.size(),
                0,
                reinterpret_cast<sockaddr*>(&peer_addr_),
                sizeof(peer_addr_)
        );
        if(bytesSent<0){
            char *p = strerror(errno);
            std::cout << peer_addr_.sin_port << std::endl;
            std::cout << peer_addr_.sin_addr.s_addr << std::endl;
            throw std::runtime_error("GBNSocket: failed to sent package" + std::string(p,strlen(p)));
        }
        sentq.pop_front();
    }
}

size_t GBNSocket::Read(char *buf, size_t n) {
    // TODO: need support handling write to ended pipe (fin)
    std::unique_lock<std::mutex> lock(read_mtx_);
    while(connection_.GetRecvStream().IsEmpty())
        read_cv_.wait(lock);

    auto bytes_reads = connection_.GetRecvStream().Read(buf,n);
    return bytes_reads;
}

size_t GBNSocket::Write(const char *buf, size_t n) {
    // TODO: need support handling write to ended pipe (fin)

    auto bytes_writes = connection_.Write(buf,n);
    return bytes_writes;
}

size_t GBNSocket::Write(const std::string &str) {
    return Write(str.c_str(),str.size());
}

std::string GBNSocket::Read(size_t n) {
    std::unique_lock<std::mutex> lock(read_mtx_);
    while(connection_.GetRecvStream().IsEmpty())
        read_cv_.wait(lock);

    auto retstring = connection_.GetRecvStream().Read(n);
    return retstring;
}


