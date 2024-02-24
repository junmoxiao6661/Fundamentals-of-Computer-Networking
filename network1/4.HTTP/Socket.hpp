#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include "Log.hpp"
#include <cstring>

enum
{
    SocketErr = 2,
    BindErr,
    ListenErr
};
const int backlog = 10;

class Sock
{
public:
    Sock()
    {}
    void Socket()
    {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0)
        {
            lg(Fatal, "socket error, %s: %d", strerror(errno), errno);
            exit(SocketErr);
        }
    }
    void Bind(const uint16_t port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;
        if (bind(sockfd_, (struct sockaddr *)&(local), sizeof(local)) < 0)
        {
            lg(Fatal, "bind error, %s: %d", strerror(errno), errno);
            exit(BindErr);
        }
    }
    void Listen()
    {
        if (listen(sockfd_, backlog) < 0)
        {
            lg(Fatal, "listen error, %s: %d", strerror(errno), errno);
            exit(ListenErr);
        }
    }
    int Accept(std::string *clientip, uint16_t *clientport)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int newfd = accept(sockfd_, (struct sockaddr *)&(peer), &len);
        if (newfd < 0)
        {
            lg(Warning, "accept error, %s: %d", strerror(errno), errno);
            return -1;
        }
        char ipstr[64];
        inet_ntop(AF_INET,&peer.sin_addr,ipstr,sizeof(ipstr));
        *clientip=ipstr;
        *clientport=ntohs(peer.sin_port);
        return newfd;
    }
    bool Connect(const std::string &ip,const uint16_t &port) 
    {
        struct sockaddr_in peer;
        memset(&peer,0,sizeof(peer));
        peer.sin_family=AF_INET;
        peer.sin_port=htons(port);
        inet_pton(AF_INET,ip.c_str(),&peer.sin_addr);
        
        int n = connect(sockfd_,(struct sockaddr*)&peer,sizeof(peer));
        if(n<0)
        {
            std::cerr << "connect to"<< ip <<":" <<port <<std::endl;
            return false;
        }
        return true;
    }
    void Close()
    {
        close(sockfd_);
    }
    int Fd()
    {
        return sockfd_;
    }
    ~Sock()
    {
    }

private:
    int sockfd_;
};
