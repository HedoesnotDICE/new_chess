#include "tool.h"
#include <string>
#include <iostream>

std::string Tool::Sockaddr2Str(const sockaddr_in src)
{
    std::string ret;
    u_int32_t addr = src.sin_addr.s_addr;
    unsigned char temp;
    temp = addr;
    ret += std::to_string(temp);
    ret += '.';

    temp = addr >> 8;
    ret += std::to_string(temp);
    ret += '.';

    temp = addr >> 16;
    ret += std::to_string(temp);
    ret += '.';

    temp = addr >> 24;
    ret += std::to_string(temp);
    
    return ret;
}