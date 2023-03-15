#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <signal.h> 
#include "tool.h"

using namespace std;

int main()
{
    int ret;
    bool client1 = true;
    std::string passiveIp;

    const char* ip = "192.168.1.11"; 
    char message[16]; 
    int port = 9899;

    //create an ipv4 socket address
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sockFd = socket(PF_INET, SOCK_STREAM, 0);
    if(sockFd == -1)
    {
       	 std::cout << "Create socket error!" << endl;
    }

    ret = bind(sockFd, (struct sockaddr*)&address, sizeof(address));
    if(ret == -1)
    {
	    std::cout << "Bind socket error!"<< endl;
    }

    ret = listen(sockFd, 5);
    if(ret == -1)
    {
	    std::cout << "Listen error!" << endl;
    }
    std::cout << "listen ok" << endl;
    sleep(10);

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof(client);
    while(true)
    {
        int connfd = accept(sockFd, (struct sockaddr*)&client, &client_addrlength);
        if(connfd < 0)
        {
		std::cout << "Accept error" << endl;
        }
        else
        {

   	   	std::cout << "Accept success" << endl;
	}
    }

    close(sockFd);

    printf("exit!");

    return 0;
}
