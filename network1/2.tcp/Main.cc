#include "TcpServer.hpp"
#include <iostream>
#include <memory>
#include <string>

void Usage(std::string proc)
{
    std::cout << "\n\rUsage: " << proc << " port[1024+]\n"
              << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }
    uint16_t port = std::stoi(argv[1]);
    std::unique_ptr<TcpServer> svr(new TcpServer(port));
    svr->InitServer();
    svr->Start();
    return 0;
}