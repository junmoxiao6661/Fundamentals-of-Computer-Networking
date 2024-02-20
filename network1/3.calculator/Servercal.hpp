#pragma once
#include <iostream>
#include "Protocol.hpp"
#include <string>
enum{
    Div_Zero =1,
    Mode_Zero,
    Other_Oper
};

class Servercal{
public:
    Servercal(){}
    Response CalculatorHelper(const Request req)
    {
        Response resp(0,0);
        switch(req.op)
        {
            case '+' :
                resp.result=req.x+req.y;
                break;
            case '-' :
                resp.result=req.x-req.y;
                break;
            case '*' :
                resp.result=req.x*req.y;
                break;
            case '/' :
                if(req.y==0) resp.code =Div_Zero;
                resp.result=req.x/req.y;
                break;
            case '%' :
                if(req.y==0) resp.code = Mode_Zero;
                resp.result=req.x%req.y;
                break;
            default:
                resp.code =Other_Oper;
                break;
        }
        return resp;

    }
    std::string Calculator(std::string &package)
    {
        std::string content;
        bool r =Decode(package,&content);
        if(!r) return "";
        Request req;
        r=req.Deserialize(content);// 反序列化
        if(!r) return "";
        Response resp=CalculatorHelper(req);// 计算
        resp.serialize(&content);// 序列化
        content=Encode(content);
        return content;


    }

    ~Servercal(){}

};