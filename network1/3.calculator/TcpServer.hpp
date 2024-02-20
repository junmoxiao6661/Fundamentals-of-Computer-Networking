#pragma once
#include "Log.hpp"
#include "Socket.hpp"
#include <iostream>
#include <signal.h>
#include <functional>
#include <string>

using func_t = std::function<std::string (std::string &package)>;
class TcpServer{
public:
    TcpServer(uint16_t port,func_t callback):port_(port),callback_(callback)
    {}
    bool InitServer()
    {
        listensock_.Socket();
        listensock_.Bind(port_);
        listensock_.Listen();
        lg(Info,"Init server ... done"); 
        return true;
    }
    void Start()
    {
        signal(SIGCHLD,SIG_IGN);
        signal(SIGPIPE,SIG_IGN);
        while(true)
        {
            std::string clientip;
            uint16_t clientport;
            int sockfd = listensock_.Accept(&clientip,&clientport);
            if(sockfd<0) continue;// 重连
            lg(Info,"accept a new link, sockfd: %d, clientip: %s, clientport: %d",sockfd,clientip.c_str(),clientport);

            // 提供服务
            if(fork()==0)//多进程
            {
                listensock_.Close();
                //计算
                std::string inbuffer_stream;
                while(true)
                {
                    char buffer[128];
                    ssize_t n =read(sockfd,buffer,sizeof(buffer));
                    if(n>0)
                    {
                        buffer[n]=0;
                        inbuffer_stream+=buffer;
                        std::string info =callback_(inbuffer_stream);
                        if(info.empty()) continue;
                        write(sockfd,info.c_str(),info.size());

                    }
                }
                exit(0);
            }
            close(sockfd);


        }

    }
    ~TcpServer()
    {}
private:
    uint16_t port_;
    Sock listensock_;
    func_t callback_;
};
