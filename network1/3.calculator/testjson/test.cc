#include <iostream>
#include <jsoncpp/json/json.h>
#include <string>

int main()
{
    Json::Value root;
    root["x"]=100;
    root["y"]=200;
    root["op"]='+';
    Json::StyledWriter w;
    std::string res=w.write(root);
    std::cout<<res<<std::endl;

    Json::Value v;
    Json::Reader r;
    r.parse(res,v);
    int x =v["x"].asInt();
    int y =v["y"].asInt();
    char op = v["op"].asInt();
    std::cout<<x<<std::endl;
    std::cout<<y<<std::endl;
    std::cout<<op<<std::endl;

}
