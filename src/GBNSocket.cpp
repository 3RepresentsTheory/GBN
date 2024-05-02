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
    peer_addr_.sin_port =0;
    peer_addr_.sin_addr.s_addr=0;

    // event register:

    // basic udp listening event
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

    // udp socket write event
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


    // user socket read event
    int rrequest_listen_fd = rrequest_.GetReadfd();
    int rreply_fd = rreply_.GetWritefd();
    eventloop_.RegisterEvent(
        rrequest_listen_fd,
        EPOLLIN,
        [rrequest_listen_fd,rreply_fd,this]{
            char* usr_buf; size_t n,bytes_read=0;
            read(rrequest_listen_fd,&usr_buf,sizeof(usr_buf));
            read(rrequest_listen_fd,&n,sizeof(n));

            // only work when the stream isn't empty
            if(!connection_.GetRecvStream().IsEmpty()){
                bytes_read = connection_.Read(usr_buf,n);
                write(rreply_fd,&bytes_read,sizeof(bytes_read));
                // notify the socket reader
                {
                    std::unique_lock<std::mutex> lk(read_mt_);
                    rstate_ = PASS;
                    read_cv_.notify_one();
                }
            }else{
                {
                    std::unique_lock<std::mutex> lk(write_mt_);
                    rstate_ = WCHGE;
                }
            }
        }
    );

    // user socket write event
    int wrequest_listen_fd = wrequest_.GetReadfd();
    int wreply_fd = wreply_.GetWritefd();
    eventloop_.RegisterEvent(
        wrequest_listen_fd,
        EPOLLIN,
        [wrequest_listen_fd,wreply_fd,this]{
            char* usr_buf; size_t n,bytes_write=0;
            read(wrequest_listen_fd,&usr_buf,sizeof(usr_buf));
            read(wrequest_listen_fd,&n,sizeof(n));

            fprintf(stderr,"server response write event at %p with %zu ",usr_buf,n);

            // slightly imprecise here, when its not full, the write may exceed the buf length
            if(!connection_.IsSenderFull()){
                fprintf(stderr,"with success\n");
                bytes_write = connection_.Write(usr_buf,n);
                write(wreply_fd,&bytes_write,sizeof(bytes_write));
                {
                    std::unique_lock<std::mutex> lk(write_mt_);
                    wstate_ = PASS;
                    write_cv_.notify_one();
                }
            }else{
                fprintf(stderr,"but failed, give up\n");
                {
                    std::unique_lock<std::mutex> lk(write_mt_);
                    wstate_ = WCHGE;
                }
            }
        }
    );


    // launch the thread for event loop:
    eventthread_ = std::thread(&EventLoop::Loop,&eventloop_);

}

GBNSocket::~GBNSocket() {
    eventloop_.StopLoop();
    eventthread_.join();
}
static const char*n2hex = "0123456789ABCDEF";

constexpr int BUFFER_SIZE = 65507 ; // max udp package
void GBNSocket::FrameReceived(int socketfd) {

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

    fprintf(stderr,
            "frame received! with raw data: \n"
    );
    for (int j = 0; j < bytesRead; ++j) {
        unsigned char x = buffer[j];
        fprintf(stderr,"%c%c",n2hex[(x&0xF0)>>4],n2hex[x&0xF]);
        if(j % 2 == 1) fprintf(stderr, " ");
    }
    fprintf(stderr,"\n");

    auto && temp_pkg =GBNPDU(buffer,bytesRead);
    if(!temp_pkg.IsFormated()){
        std::cout << "detect unformated package" << std::endl;
        return;
    }

    if(temp_pkg.ackf_){
        fprintf(stderr,
                "frame received! with PDU ack: num %d\n\n",
                temp_pkg.GetNum()
        );
    }else{
        fprintf(stderr,
                "frame received! with PDU data: num %d, len %zu, and data: \n",
                temp_pkg.GetNum(),
                temp_pkg.GetLen()
        );

        for (int j = 0; j < temp_pkg.GetLen(); ++j) {
            unsigned char x = temp_pkg.GetData()[j];
            fprintf(stderr,"%c%c",n2hex[(x&0xF0)>>4],n2hex[x&0xF]);
            if(j % 2 == 1) fprintf(stderr, " ");
        }
        fprintf(stderr,"\n\n");
    }

    connection_.PkgReceived(temp_pkg);

    // wake up possible socket reader
    // notify the socket reader
    if(!connection_.GetRecvStream().IsEmpty())
    {
        std::unique_lock<std::mutex> lk(read_mt_);
        if(rstate_==WCHGE){
            rstate_ = RETRY;
            read_cv_.notify_one();
        }
    }

    if(!connection_.IsSenderFull()){
        std::unique_lock<std::mutex> lk(write_mt_);
        if(wstate_==WCHGE){
            wstate_ = RETRY;
            write_cv_.notify_one();
        }
    }

    if(peer_addr_.sin_port==0)
        peer_addr_ = clientAddr;
}


void GBNSocket::FrameSent(int eventfd) {
    if(peer_addr_.sin_port==0)
        throw std::runtime_error("GBNSocket: set peer address first!");
    int sentnum;
    // temp no checking
    read(eventfd,&sentnum,sizeof(sentnum));

//    fprintf(stderr,"frame sent, there is %d pkg wait to sent\n",sentnum);

    int sockfd = sfd_.GetFd();
    auto & sentq = connection_.GetWaitToSent();
    for (int i = 0; i < sentnum && !sentq.empty(); ++i) {
        auto &pkg = sentq.front();

        if(pkg.ackf_){
            fprintf(stderr,
                    "frame sent! with PDU ack: num %d\n"
                    "to the host: %s:%d\n\n",
                    pkg.GetNum(),
                    inet_ntoa(peer_addr_.sin_addr),
                    ntohs(peer_addr_.sin_port)
            );
        }else{
            fprintf(stderr,
                    "frame sent! with PDU data: num %d, len %zu, and data: ",
                    pkg.GetNum(),
                    pkg.GetLen()
            );
            for (int j = 0; j < pkg.GetLen(); ++j) {
                unsigned char x = pkg.GetData()[j];
                fprintf(stderr,"%c%c",n2hex[(x&0xF0)>>4],n2hex[x&0xF]);
                if(j % 2 == 1) fprintf(stderr, " ");
            }
            fprintf(stderr,"\n\n");
        }
//
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

    int rrequest_fd = rrequest_.GetWritefd();
    int rreply_fd   = rreply_.GetReadfd();
    size_t bytes_read;

    bool need_retry = false;
    do{
        write(rrequest_fd,&buf,sizeof(buf));
        write(rrequest_fd,&n,sizeof(n));

        // wait for data ready
        {
            std::unique_lock<std::mutex> lk(read_mt_);
            read_cv_.wait(lk,[this]{return rstate_==PASS||rstate_==RETRY;});
            need_retry = (rstate_==RETRY);
            rstate_ = READY;
        }
    }while(need_retry);

    read(rreply_fd,&bytes_read,sizeof(bytes_read));

    return bytes_read;
}

size_t GBNSocket::Write(const char *buf, size_t n) {
    // TODO: need support handling write to ended pipe (fin)

    int wrequest_fd = wrequest_.GetWritefd();
    int wreply_fd   = wreply_.GetReadfd();
    size_t bytes_write;

    bool need_retry = false;
    do{
        write(wrequest_fd,&buf,sizeof(buf));
        write(wrequest_fd,&n,sizeof(n));

        fprintf(stderr,"user request write event at %p with %zu ",buf,n);

        // wait for write data ready
        {
            std::unique_lock<std::mutex> lk(write_mt_);
            write_cv_.wait(lk,[this]{return wstate_==PASS||wstate_==RETRY;});
            need_retry = (wstate_==RETRY);
            wstate_ = READY;
        }
    }while(need_retry);

    read(wreply_fd,&bytes_write,sizeof(bytes_write));

    return bytes_write;
}

size_t GBNSocket::Write(const std::string &str) {
    return Write(str.c_str(),str.size());
}

std::string GBNSocket::Read(size_t n) {

    auto retstring = connection_.GetRecvStream().Read(n);
    return retstring;
}


