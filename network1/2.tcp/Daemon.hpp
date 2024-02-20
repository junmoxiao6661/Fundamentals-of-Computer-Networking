#pragma oncce
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const std::string nullfile = "/dev/null";
void Daemon(const std::string cwd = "")
{
    // 1.忽略其他异常信号
    signal(SIGCLD,SIG_IGN);
    signal(SIGPIPE,SIG_IGN);
    signal(SIGSTOP,SIG_IGN);

    // 2.将自己变为独立会话
    if(fork()>0) exit(0);
    setsid();

    // 3.更改当前调用进程
    if(!cwd.empty()) chdir(cwd.c_str());

    // 4.标准输入、输出、错误重定向至 /dev/null
    int fd =open(nullfile.c_str(),O_RDWR);
    if(fd>0)
    {
        dup2(fd,0);
        dup2(fd,1);
        dup2(fd,2);
        close(fd);
    }




}