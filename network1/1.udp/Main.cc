#include "UdpServer.hpp"
#include <memory>
#include <cstdio>
#include <vector>
#include <string>
#include "Log.hpp"
Log log;
void Usage(std::string proc)
{
    std::cout<<"\n\rUsage: "<<proc<<" port[1024+]\n"<<std::endl;
}
std::string Handler(const std::string &str,const std::string& clientip,uint16_t clientport)
{
    std::cout<<"["<<clientip<<":"<<clientport<<"]#"<<str<<std::endl;
    std::string res=" Server get amessage: ";
    res+=str;
    std::cout<<res<<std::endl;
    return res;
}

bool SafeCheck(const std::string cmd)
{
    std::vector<std::string> key_word={
        "rm",
        "mv",
        "cp",
        "kill",
        "sudo",
        "unlink",
        "uninstall",
        "yum",
        "top",
        "while"
    };
    for(auto& word:key_word)
    {
        auto pos=cmd.find(word);
        if(pos!=std::string::npos) return false;
    }
    return true;
}

std::string ExcuteCommand(const std::string &cmd)
{
    if(!SafeCheck(cmd)) return "Bad man";

    FILE  *fp=popen(cmd.c_str(),"r");
    if(fp==nullptr)
    {
        perror("popen");
        return "error";
    }
    std::string result;
    char buffer[4096];
    while(true)
    {
        char *res=fgets(buffer,sizeof(buffer),fp);
        if(res==nullptr)    break;
        result+=buffer;
    }
    pclose(fp);
}
int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        Usage(argv[0]);
        exit(0);
    }
    uint16_t port=std::stoi(argv[1]);
    std::unique_ptr<UdpServer> svr(new UdpServer(port));
    svr->Init();
    svr->Run(Handler);
    return 0;
}
