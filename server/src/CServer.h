#ifndef CSERVER_H
#define CSERVER_H

#include <iostream>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <map>
#include <queue>
#include <vector>

#include "CMysql.h"
#include "CBalanceThread.h"
#include "net_msg.pb.h"
#include "my_types.h"
#include "tool.h"

//thread routine
void * ThreadRoutine(void *);


//configure for threads
#define     TASK_THREAD_NUM    2

//configure for serverlog
const std::string LogInitFileName = "./log.conf";

//configure for network
const std::string ServerIP  =   "192.168.1.9";
const int ServerPort        =   9899;
#define     LISTENPENDINGNUM    10
#define     MATCHREQBUFSIZE     1024
#define     BALANCEREQBUFSIZE   1024

struct SWaitLock
{
    pthread_mutex_t wait_lock;
    bool is_waiting;
    std::string ip;
    unsigned int uid;
    int lock() { return pthread_mutex_lock(&wait_lock); }
    int try_lock() { return pthread_mutex_trylock(&wait_lock); }
    int unlock() { return pthread_mutex_unlock(&wait_lock); }
};
struct SGlobalSeq
{
    pthread_mutex_t seq_lock;
    unsigned int global_seq;
    int lock() { return pthread_mutex_lock(&seq_lock); }
    int try_lock() { return pthread_mutex_trylock(&seq_lock); }
    int unlock() { return pthread_mutex_unlock(&seq_lock); }
};
struct SMatchInfo
{

    unsigned int game_seq, uid1, uid2;
    SMatchInfo(unsigned int id1 = 0, unsigned int id2 = 0) { uid1 = id1; uid2 = id2; }
};
struct SMatchQueue
{
    pthread_mutex_t match_queue_lock;
    std::map<unsigned int, struct SMatchInfo *> Mp_match_infos;
    int lock() { return pthread_mutex_lock(&match_queue_lock); }
    int try_lock() { return pthread_mutex_trylock(&match_queue_lock); }
    int unlock() { return pthread_mutex_unlock(&match_queue_lock); }
};

struct SMatchThreadParam
{   
    int fd;
    sockaddr_in server;
};

struct STaskQueue
{
    pthread_mutex_t taskqueue_lock;
    std::queue<SMatchThreadParam *> taskqueue;
    void push(SMatchThreadParam * param)
    {
        pthread_mutex_lock(&taskqueue_lock);
        taskqueue.push(param);
        pthread_mutex_unlock(&taskqueue_lock);
    }
    SMatchThreadParam * pop()
    {
        pthread_mutex_lock(&taskqueue_lock);
        if(taskqueue.size() == 0)
        {
            pthread_mutex_unlock(&taskqueue_lock);
            return NULL;
        }
        auto ret = taskqueue.front();
        taskqueue.pop();
        pthread_mutex_unlock(&taskqueue_lock); 
        return ret;
    }
};

class CServer
{
    static CServer * instance;
    CServer();

    //interfaces for mysql
    CMysql * mysql;

    //interfaces for network
    int listenfd;
    bool InitListen();

    //task queue
    STaskQueue * task_queue;
    bool InitTaskQueue();

    //match information
    SWaitLock * swait_lock;
    SGlobalSeq * game_seq;
    SMatchQueue * match_queue;
    bool InitMatchQueue();

    //task thread
    std::vector<pthread_t> task_threads;
    msg::Login_info net_msg;
    bool InitTaskThreads();
    friend void * ThreadRoutine(void *);
    bool sign_in(const std::string & name, const std::string & passwd, msg::Login_info & net_msg);
    bool sign_up(std::string & name, std::string & passwd, msg::Login_info & net_msg);
    bool match(const std::string & name, int connfd, SMatchThreadParam * match_param);
    bool balance(unsigned int seq, unsigned int uid, bool won);

public:
    log4cpp::Category * ServerLog;    
    bool Init();
    static CServer * GetInstance();
    bool InitLog(std::string logInitFile);
    void Start();
    bool StartTaskThread();

};



#endif