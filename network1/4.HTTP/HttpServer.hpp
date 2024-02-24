#pragma once
#include <iostream>
#include "Socket.hpp"
#include <string>
#include "Log.hpp"
#include <pthread.h>
#include <fstream>
#include <vector>
#include <sstream>
const std::string sep = "\r\n";
const std::string wwwroot="./wwwroot";
class HttpServer;
class ThreadData
{
public:
    ThreadData(int fd) : sockfd(fd)
    {
    }
    int sockfd;
};
class HttpRequest{
public:
    void Deserialize(std::string req)
    {
        while(true)
        {
            std::size_t pos = req.find(sep);
            if(pos==std::string::npos) break;
            std::string temp = req.substr(0,pos);
            if(temp.empty()) break;
            req_header.push_back(temp);
            req.erase(0,pos+sep.size());
        }
        text = req;
    }
    void Parse()
    {
        std::stringstream ss(req_header[0]);
        ss>>method>>url>>http_version;
    }
public:
    std::vector<std::string> req_header;
    std::string text;

    std::string method;
    std::string url;
    std::string http_version;
};
class HttpServer
{
public:
    HttpServer(uint16_t port) : port_(port)
    {
    }
    bool Start()
    {
        listensock_.Socket();
        listensock_.Bind(port_);
        listensock_.Listen();
        while (true)
        {
            std::string clientip;
            uint16_t clientport;
            int sockfd = listensock_.Accept(&clientip, &clientport);
            lg(Info, "accept a new link, clientip:%s, clientport:%d, sockfd:%d", clientip.c_str(), clientport, sockfd);
            pthread_t tid;
            ThreadData *td = new ThreadData(sockfd);
            pthread_create(&tid, nullptr, ThreadRun, td);
        }
    }
    static std::string ReadHtmlContent(const std::string &htmlpath)
    {
        std::ifstream in(htmlpath);
        if(!in.is_open()) return "404";
        std::string line;
        std::string content;
        while(std::getline(in,line))
        {
            content+=line;
        }
        in.close();
        return content;
    }
    static void HandlerHttp(int sockfd)
    {
        char buffer[102400];
        ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << buffer;
            std::string text = ReadHtmlContent("wwwroot/index.html");
            std::string response_line = "HTTP/1.0 200 OK\r\n";
            std::string response_header = "Content-length: ";
            response_header += std::to_string(text.size());
            response_header += "\r\n";
            std::string blank_line = "\r\n";
            std::string response = response_line;
            response += response_header;
            response += blank_line;
            response += text;
            send(sockfd, response.c_str(), response.size(), 0);
        }
    }
    static void *ThreadRun(void *args)
    {
        // 无this指针
        ThreadData *td = static_cast<ThreadData *>(args);
        pthread_detach(pthread_self());

        HandlerHttp(td->sockfd); // 静态成员函数无this
        close(td->sockfd);
        return nullptr;
    }

private:
    Sock listensock_;
    uint16_t port_;
};
