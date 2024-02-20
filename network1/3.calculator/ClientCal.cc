#include "Log.hpp"
#include "TcpServer.hpp"
#include <iostream>
#include <string>
#include <ctime>
#include "Protocol.hpp"

void Usage(std::string proc)
{
    std::cout << "\n\rUsage: " << proc << " port[1024+]\n"
              << std::endl;
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);
    Sock sockfd;
    sockfd.Socket();
    bool r = sockfd.Connect(serverip, serverport);
    if (!r)
        return 1;
    srand(time(nullptr) ^ getpid());
    std::string inbuffer_stream;
    int cnt = 1;
    const std::string opers = "+-*/%";
    while (cnt <= 20)
    {
        int x = rand() % 100 + 1;
        usleep(1000);
        int y = rand() % 100;
        usleep(1000);
        char oper = opers[rand() % opers.size()];
        Request req(x, y, oper);
        std::string package;
        req.Serialize(&package);
        package = Encode(package);
        write(sockfd.Fd(), package.c_str(), package.size());

        char buffer[128];
        ssize_t n = read(sockfd.Fd(), buffer, sizeof(buffer));
        if (n > 0)
        {
            buffer[n] = 0;
            inbuffer_stream += buffer;
            std::string content;
            bool r = Decode(inbuffer_stream, &content);
            Response resp;
            r = resp.Deserialize(content);
            std::cout << req.x << " " << req.op << " " << req.y << " "
                      << "="
                      << " " << resp.result << " "
                      << "code: " << resp.code << std::endl;
        }
        sleep(1);
        cnt++;
    }
    sockfd.Close();
    return 0;
}