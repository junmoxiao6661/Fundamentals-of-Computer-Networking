#pragma once
#include <iostream>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include <unordered_map>
#include "Log.hpp"

using func_t = std::function<std::string(const std::string &,const std::string& ,uint16_t )>;
Log lg;
const int size = 1024;
enum
{
    SOCKET_ERR = 1,
    BIND_ERR = 2,
};
uint16_t defaultport = 8080;
std::string defaultip = "0.0.0.0";

class UdpServer
{
public:
    UdpServer(const uint16_t &port = defaultport, const std::string &ip = defaultip) : sockfd_(0), port_(port), ip_(ip), isrunning_(false)
    {
    }
    void Init()
    {
        // 1.创建udp套接字
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd_ < 0)
        {
            lg(Fatal, "socket create error, sockfd: %d", sockfd_);
            exit(SOCKET_ERR);
        }
        lg(Info, "socket create success, sockfd: %d", sockfd_);
        // 2. bind socket
        struct sockaddr_in local;
        bzero(&local, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port_); // 保证端口号是网络字节序
        local.sin_addr.s_addr = inet_addr(ip_.c_str());
        // local.sin_addr.s_addr=INADDR_ANY;
        if (bind(sockfd_, (const struct sockaddr *)&local, sizeof(local)) < 0)
        {
            lg(Fatal, "bind error,errno: %d, err string: %s", errno, strerror(errno));
            exit(BIND_ERR);
        }
        lg(Info, "bind sucess,errno: %d, err string: %s", errno, strerror(errno));
    }
    void CheckUser(const struct sockaddr_in &client,const std::string &clientip,uint16_t clientport)
    {
        auto iter =online_user.find(clientip);
        if(iter==online_user.end())
        {
            online_user.insert({clientip,client});
            std::cout<<"["<<clientip<<":"<<clientport<<"]add to online user"<<std::endl;
        }

    }
    void Broadcast(const struct sockaddr_in &client,const std::string &clientip,uint16_t clientport)
    {
        
    }
    void Run(func_t func)
    {
        isrunning_ = true;
        char inbuffer[size];
        while (isrunning_)
        {
            ssize_t n = recvfrom(sockfd_, inbuffer, sizeof(inbuffer) - 1, /*为末尾留'\0'*/ 0, (struct sockaddr *)&client, &len);
            if (n < 0)
            {
                lg(Warning, "recvfrom error,errno: %d, err string: %s", errno, strerror(errno));
                continue;
            }
            uint16_t clientport = ntohs(client.sin_port);
            std::string clientip=inet_ntoa(client.sin_addr);

            CheckUser(client,clientip,clientport);
            std::string info=inbuffer;
            Broadcast();
            // inbuffer[n] = 0;
            // std::string info = inbuffer;
            // std::string echo_string = func(info,clientip,clientport);
            // sendto(sockfd_, echo_string.c_str(), echo_string.size(), 0, (const sockaddr *)&client, len);
        }
    }

    ~UdpServer()
    {
        if (sockfd_ > 0)
            close(sockfd_);
    }

private:
    int sockfd_;
    std::string ip_;
    uint16_t port_;
    bool isrunning_;
    std::unordered_map<std::string,struct sockaddr_in> online_user;
};