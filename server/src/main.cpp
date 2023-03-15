#include "CServer.h"
static CServer * server;
int main()
{
    server = CServer::GetInstance();
    server->Init();
    server->Start();

}