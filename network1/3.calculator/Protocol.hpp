#pragma once

#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>
// #define Myself 1
const std::string blank_space_sep = " ";
const std::string protocol_sep = "\n";
std::string Encode(std::string &content)
{
    std::string package = std::to_string(content.size());
    package += protocol_sep;
    package += content;
    package += protocol_sep;
    return package;
}
bool Decode(std::string &package, std::string *comment)
{
    std::size_t pos = package.find(protocol_sep);
    if (pos == std::string::npos)
        return false;
    std::string len_str = package.substr(0, pos);
    std::size_t len = std::stoi(len_str);
    ssize_t total_len = len + len_str.size() + 2;
    if (package.size() < total_len)
        return false;
    *comment = package.substr(pos + 1, len);

    package.erase(0, total_len);
    return true;
}
class Request
{
public:
    Request()
    {
    }
    Request(int data1, int data2, char oper) : x(data1), y(data2), op(oper)
    {
    }
    bool Serialize(std::string *out)
    {
#ifdef Myself
        // 构建报文有效载荷
        //  struct to string "len"\n"x op y"
        std::string s = std::to_string(x);
        s += blank_space_sep;
        s += op;
        s += blank_space_sep;
        s += std::to_string(y);
        // 封装报头
        *out = s;
        return true;
#else
        Json::Value root;
        root["x"] = x;
        root["y"] = y;
        root["op"] = op;
        Json::StyledWriter w;
        *out = w.write(root);
        return true;
#endif
    }
    bool Deserialize(const std::string &in)
    {
#ifdef MySelf
        std::size_t left = in.find(blank_space_sep);
        if (left == std::string::npos)
            return false;
        std::string part_x = in.substr(0, left);

        std::size_t right = in.rfind(blank_space_sep);
        if (right == std::string::npos)
            return false;
        std::string part_y = in.substr(right + 1);

        if (left + 2 != right)
            return false;
        op = in[left + 1];
        x = std::stoi(part_x);
        y = std::stoi(part_y);
        return true;
#else
        Json::Value root;
        Json::Reader r;
        r.parse(in, root); // 读入root
        x = root["x"].asInt();
        y = root["y"].asInt();
        op = root["op"].asInt();
        return true;
#endif
    }

public:
    int x;
    int y;
    char op; //+ - * / %
};

class Response
{
public:
    Response(int res, int c) : result(res), code(c)
    {
    }
    Response()
    {
    }
    bool serialize(std::string *out)
    {
#ifdef MySelf
        std::string s = std::to_string(result);
        s += blank_space_sep;
        s += std::to_string(code);
        *out = s;
        return true;
#else
        Json::Value root;
        root["result"] = result;
        root["code"] = code;
        Json::StyledWriter w;
        *out = w.write(root);
        return true;
#endif
    }
    bool Deserialize(const std::string &in)
    {
#ifdef MySelf
        std::size_t pos = in.find(blank_space_sep);
        if (pos == std::string::npos)
            return false;
        std::string part_left = in.substr(0, pos);
        std::string part_right = in.substr(pos + 1);
        result = std::stoi(part_left);
        code = std::stoi(part_right);
        return true;
#else
       Json::Value root;
       Json::Reader r;
       r.parse(in,root);
       result = root["result"].asInt();
       code = root["code"].asInt();
       return true;
#endif
    }

public:
    int result;
    int code; // 0可信 其他错误
};