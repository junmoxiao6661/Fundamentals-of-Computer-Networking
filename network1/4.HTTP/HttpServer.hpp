#pragma once
#include <iostream>
#include "Socket.hpp"
#include <string>
#include "Log.hpp"
#include <pthread.h>
struct ThreadData{
    int sockfd;
};
class HttpServer{
public:
    HttpServer(uint16_t port):port_(port)
    {}
    bool Start()
    {
        listensock_.Socket();
        listensock_.Bind(port_);
        listensock_.Listen();
        while(true)
        {
            std::string clientip;
            uint16_t clientport;
            int sockfd=listensock_.Accept(&clientip,&clientport);
            lg(Info,"accept a new link, clientip:%s, clientport:%d, sockfd:%d",clientip.c_str(),clientport,sockfd);
            pthread_t tid;
            ThreadData *td =new ThreadData;
            td->sockfd=sockfd;
            pthread_create(&tid,nullptr,ThreadRun,td);

        }
    }
    static void *ThreadRun(void *args)
    {
        // 无this指针
        ThreadData  *td=static_cast<ThreadData*>(args);
        pthread_detach(pthread_self());
        char buffer[10240];
        ssize_t n = recv(td->sockfd,buffer,sizeof(buffer)-1,0);
        if (n>0)
        {
            buffer[n]=0;
            std::cout<<buffer<<std::endl;
        }
        close(td->sockfd);
        return nullptr;
    }
private:
    Sock listensock_;
    uint16_t port_;
};