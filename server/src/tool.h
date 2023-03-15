#ifndef TOOL_H__
#define TOLL_H__

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
class Tool
{
public:
    static std::string Sockaddr2Str(const sockaddr_in src);
};

#endif