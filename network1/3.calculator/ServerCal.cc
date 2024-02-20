#include "Log.hpp"
#include "TcpServer.hpp"
#include "Protocol.hpp"
#include "Servercal.hpp"

void Usage(std::string proc)
{
    std::cout << "\n\rUsage: " << proc << " port[1024+]\n"
              << std::endl;
}

int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        Usage(argv[0]);
        exit(0);
    }
    uint16_t port = std::stoi(argv[1]);
    Servercal cal;
    TcpServer *tsvp = new TcpServer(port,bind(&Servercal::Calculator,&cal,std::placeholders::_1));
    tsvp->InitServer();
    tsvp->Start();
    return 0;
}