#include "Log.hpp"
#include "TcpServer.hpp"
#include <iostream>
#include <string>
#include <time.h>
#include "Protocol.hpp"

void Usage(std::string proc)
{
    std::cout << "\n\rUsage: " << proc << " port[1024+]\n"
              << std::endl;
}
int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        Usage(argv[0]);
        exit(0);
    }
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);
    Sock sockfd;
    sockfd.Socket();
    sockfd.Connect(serverip,serverport);
    sockfd.Close();
    return 0;
}