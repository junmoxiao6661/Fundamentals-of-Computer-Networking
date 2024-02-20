#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
        exit(1);
    }
    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);
    while (true)
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            std::cerr << "socket error" << std::endl;
            return 1;
        }
        // 客户端需要绑定，但不需要显示绑定
        // 客户端发起connect，自动随机绑定
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(serverport);
        inet_pton(AF_INET, serverip.c_str(), &(server.sin_addr));

        int n = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
        if (n < 0)
        {
            std::cerr << "connect error..." << std::endl;
        }
        std::string message;

        std::cout << "Please Enter# ";
        std::getline(std::cin, message);
        write(sockfd, message.c_str(), message.size());
        char inbuffer[4096];
        int n = read(sockfd, inbuffer, sizeof(inbuffer));
        if (n > 0)
        {
            inbuffer[n] = 0;
            std::cout << inbuffer << std::endl;
        }
        close(sockfd);
    }
    
    return 0;
}