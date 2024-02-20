#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "Log.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"
#include "Daemon.hpp"

const int defaultfd = -1;
const std::string defaultip = "0.0.0.0";
const int backlog = 5;
Log lg;
enum
{

    SocketError = 2,
    BindError,
    ListenError,

};
class TcpServer;
class ThreadData
{
public:
    ThreadData(int fd, const std::string &ip, const uint16_t &p, TcpServer *t) : sockfd(fd), clientip(ip), clientport(p), tsvr(t)
    {}
    int sockfd;
    std::string clientip;
    uint16_t clientport;
    TcpServer *tsvr;
};

class TcpServer
{
public:
    TcpServer(const uint16_t &port, const std::string &ip = defaultip) : listensock_(defaultfd), port_(port), ip_(ip)
    {
    }
    void InitServer()
    {
        listensock_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listensock_ < 0)
        {
            lg(Fatal, "create socket, errno: %d, errstring: %s", errno, strerror(errno));
            exit(SocketError);
        }
        lg(Info, "create socket sucess, sockfd: %d", listensock_);
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_port = htons(port_);
        inet_aton(ip_.c_str(), &(local.sin_addr));
        local.sin_addr.s_addr = INADDR_ANY;
        if (bind(listensock_, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            lg(Fatal, "bind error, errno: %d, errstring: %s", errno, strerror(errno));
            exit(BindError);
        }
        lg(Info, "bind socket sucess, sockfd: %d", listensock_);
        // Tcp服务器处于等待连接的状态
        if (listen(listensock_, backlog) < 0)
        {
            lg(Fatal, "listen error, errno: %d, errstring: %s", errno, strerror(errno));
            exit(ListenError);
        }
        lg(Info, "listen socket sucess, sockfd: %d", listensock_);
    }
    void Start()
    {
        Daemon();
        ThreadPool<Task>::GetInstance()->Start();
        signal(SIGPIPE,SIG_IGN);
        // signal(SIGCHLD,SIG_IGN);
        lg(Info, "tcpserver is running...");
        while (true)
        {
            // 1.获取新连接
            struct sockaddr_in client;
            socklen_t len = sizeof(client);
            int sockfd = accept(listensock_, (struct sockaddr *)&client, &len);
            // 2.根据新连接来进行通信
            if (sockfd < 0)
            {
                lg(Warning, "accept error, errno: %d, errstring: %s", errno, strerror(errno));
                continue;
            }
            uint16_t clientport = ntohs(client.sin_port);
            char clientip[32];
            inet_ntop(AF_INET, &(client.sin_addr), clientip, sizeof(clientip));

            lg(Info, "get a new link..., sockfd: %d, client ip: %s, client port: %d", sockfd, clientip, clientport);
            // version 1 单进程
            // Service(sockfd, clientip, clientport);
            // close(sockfd);

            // version 2 多进程，但是创建进程成本太高
            // pid_t id=fork();
            // if(id==0)
            // {
            //     //child
            //     close(listensock_);
            //     if(fork()>0) exit(0);
            //     // 孤儿进程
            //     Service(sockfd, clientip, clientport);//孙子进程
            //     close(sockfd);
            //     exit(0);
            // }
            // // father
            // close(sockfd);
            // pid_t rid=waitpid(id,nullptr,0);
            // (void)rid;

            // version3 多线程
            // ThreadData *td=new ThreadData(sockfd,clientip,clientport,this);
            // pthread_t tid;
            // pthread_create(&tid, nullptr, Routine, td);

            //version 4 线程池
            Task t(sockfd,clientip,clientport);
            ThreadPool<Task>::GetInstance()->Push(t);
        }
    }
    static void *Routine(void *args)
    {
        pthread_detach(pthread_self());
        ThreadData *td=static_cast<ThreadData*>(args);
        td->tsvr->Service(td->sockfd,td->clientip,td->clientport);
        delete td;
        return nullptr;

    }
    void Service(int sockfd, const std::string &clientip, const uint16_t &clientport)
    {
        char buffer[4096];
        while (true)
        {
            ssize_t n = read(sockfd, buffer, sizeof(buffer));
            if (n > 0)
            {
                buffer[n] = 0;
                std::cout << "client say# " << buffer << std::endl;
                std::string echo_string = "tcpserver echo# ";
                echo_string += buffer;
                write(sockfd, echo_string.c_str(), echo_string.size());
            }
            else if (n == 0)
            {
                lg(Info, "%s:%d quit,server close sockfd: %d", clientip.c_str(), clientport, sockfd);
                break;
            }
            else
            {
                lg(Warning, "read error, sockfd: %d, client ip: %s, client port: %d", sockfd, clientip.c_str(), clientport);
            }
        }
    }
    ~TcpServer() {}

private:
    int listensock_;
    uint16_t port_;
    std::string ip_;
};