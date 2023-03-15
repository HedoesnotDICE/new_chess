

#include "CServer.h"

CServer *  CServer::instance = NULL;

CServer::CServer()
{
    
}

bool CServer::Init()
{
    if(InitLog(LogInitFileName) == false)
    {
        std::cout << "FATAL: log init failed" << std::endl;
        exit(1);
    }

    mysql = new CMysql;
    if(mysql->Init(ServerLog) == false)
    {
        ServerLog->error("failed to init mysql connection");
        exit(2);
    }

    if(InitListen() == false)
    {
        ServerLog->error("failed to init listen");
        exit(3);
    }

    if(InitTaskQueue() == false)
    {
        ServerLog->error("failed to init taskqueue");
        exit(4);
    }

    if(InitMatchQueue() == false)
    {
        ServerLog->error("failed to init matchqueue");
        exit(5);
    }

    if(InitTaskThreads() == false)
    {
        ServerLog->error("failed to init match threads");
        exit(6);
    }

    return true;
}

CServer * CServer::GetInstance()
{
    if(instance == NULL)
    {
        instance = new CServer;
    }
    return instance;
}

bool CServer::InitLog(std::string logInitFile)
{

	log4cpp::Category& log = log4cpp::Category::getInstance("ServerLog");
    log.setPriority(log4cpp::Priority::DEBUG);
    log4cpp::PatternLayout *patternLayout = new log4cpp::PatternLayout();
    //log4cpp::Appender *appender = new log4cpp::FileAppender("test", "./log4cppTest.log");
    log4cpp::Appender *appender = new log4cpp::OstreamAppender("ServerLogForConsole", &std::cout);
    appender->setLayout(patternLayout);
    patternLayout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S.%l} [%p] %m %n");
    log.addAppender(appender);
    
    ServerLog = & log;
    return true;
}

bool CServer::InitListen()
{
    int ret;

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ServerIP.c_str(), &address.sin_addr);
    address.sin_port = htons(ServerPort);

    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if(listenfd == -1)
    {
        ServerLog->error("Create socket error!");
        return false;
    }

#ifdef MY_DEBUG
    int reuse = 1;
    ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    if(ret < 0)
    {
        ServerLog->error("Set address reuse error!");
        return false;
    }
#endif

    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
    if(ret == -1)
    {
        ServerLog->error("Bind socket error!");
        return false;
    }

    ret = listen(listenfd, LISTENPENDINGNUM);
    if(ret == -1)
    {
        ServerLog->error("Listen error!");
        return false;
    }
    ServerLog->debug("successfully listened on address: %s:%d", ServerIP.c_str(), ServerPort);
    sleep(1);
    return true;
}

bool CServer::InitTaskQueue()
{
    task_queue = new STaskQueue;
    if(pthread_mutex_init(&(task_queue->taskqueue_lock), NULL) != 0)
    {
        ServerLog->error("failed to init task_queue_lock");
        return false;
    }
    return true;
}

bool CServer::InitMatchQueue()
{
    swait_lock = new SWaitLock;
    if(swait_lock == NULL) 
    {
        ServerLog->error("failed to new wait_lock");
        return false;
    }
    if(pthread_mutex_init(&(swait_lock->wait_lock), NULL) != 0)
    {
        ServerLog->error("failed to init wait_lock");
        return false;
    }
    swait_lock->is_waiting = false;

    game_seq = new SGlobalSeq;
    if(game_seq == NULL)
    {
        ServerLog->error("failed to new game_seq");
        return false;
    }
    if(pthread_mutex_init(&(game_seq->seq_lock), NULL) != 0)
    {
        ServerLog->error("failed to init seq_lock");
        return false;
    }
    game_seq->global_seq = 0;

    match_queue = new SMatchQueue;
    if(match_queue == NULL)
    {
        ServerLog->error("failed to new match_queue");
        return false;
    }
    if(pthread_mutex_init(&(match_queue->match_queue_lock), NULL) != 0)
    {
        ServerLog->error("failed to init match_queue_lock");
        return false;
    }

    return true;
}

bool CServer::InitTaskThreads()
{
    for(int i = 0; i < TASK_THREAD_NUM; ++i)
    {
        pthread_t * tmp_tid = new pthread_t;
        if(tmp_tid == NULL)
        {
            ServerLog->error("failed to init task thread %d", i);
            return false;
        }

        task_threads.push_back(*tmp_tid);
    }
    return true;
}

void * ThreadRoutine(void * param)
{
    CServer * server = (CServer *)param;
    auto task_queue = server->task_queue;
    auto ServerLog = server->ServerLog;
    auto mysql = server->mysql;
    msg::Login_info net_msg;
    while(true)
    { 
        SMatchThreadParam * tmp_ptr = task_queue->pop();
        if(tmp_ptr == NULL) 
        {
            usleep(10000);
            continue;
        }
        SMatchThreadParam tmp = *tmp_ptr; 
        delete tmp_ptr;
        SMatchThreadParam * match_param = &tmp;
        int connfd = match_param->fd;

        if(connfd < 0)
        {
            ServerLog->error("invalid connected fd: %d", connfd);
            continue;
        }
        else
        {
            ServerLog->info("thread routine begin on fd: %d", connfd);
            char buf[MATCHREQBUFSIZE];

            int n = read(connfd, buf, MATCHREQBUFSIZE-1);
            if(n <= 0)
            {
                if(errno == EAGAIN)
                {
                    ServerLog->info("thread routine done on fd: %d", connfd);
                    close(connfd);
                    continue;
                }
                ServerLog->error("failed to read data on fd: %d, for unknown reason with errno: %d", connfd, errno);
                close(connfd);
                continue;

            }
            else 
            {   
                buf[n] = 0;
                ServerLog->debug("received %d bytes data on fd: %d:  %s", n, connfd, buf);
                std::string sock_data(buf);
                if(net_msg.ParseFromString(sock_data) == false)
                {
                    ServerLog->error("failed to parse net_msg on fd: %d", connfd);
                    close(connfd);
                    continue;
                }

                int sign_type;
                sign_type = net_msg.sign_type();
                ServerLog->info("net_msg:");
                net_msg.PrintDebugString();
                
                if(sign_type == EN_SIGN_IN)
                {
                    std::string name, passwd;
                    name = net_msg.name();
                    passwd = net_msg.passwd();
                    server->sign_in(name, passwd, net_msg);
                    
                    std::string result;
                    if(net_msg.SerializeToString(&result) == false)
                    {
                        ServerLog->error("failed to serialize net_msg on fd:%d", connfd);
                        close(connfd);
                        continue;
                    }

                    if(write(connfd, result.c_str(), MATCHREQBUFSIZE) < 0)
                    {
                        ServerLog->error("failed to write data on fd:%d", connfd);
                        close(connfd);
                        continue;
                    }
                    ServerLog->debug("write data on fd:%d : %s", connfd, result.c_str());
                }
                else if(sign_type == EN_SIGN_UP)
                {
                    std::string name, passwd;
                    name = net_msg.name();
                    passwd = net_msg.passwd();
                    server->sign_up(name, passwd, net_msg);

                    std::string result;
                    if(net_msg.SerializeToString(&result) == false)
                    {
                        ServerLog->error("failed to serialize net_msg on fd:%d", connfd);
                        close(connfd);
                        continue;
                    }

                    if(write(connfd, result.c_str(), MATCHREQBUFSIZE) < 0)
                    {
                        ServerLog->error("failed to write data on fd:%d", connfd);
                        close(connfd);
                        continue;
                    }
                    ServerLog->debug("write data on fd:%d : %s", connfd, result.c_str());
                }
                else if(sign_type == EN_NEW_GAME)
                {
                    std::string name;
                    name = net_msg.name();

                    ServerLog->info("begin to match for %s", name.c_str());
                    if(server->match(name, connfd, match_param))
                    {
                        ServerLog->info("match sccussed");
                    }
                    else 
                    {
                        ServerLog->info("match failed");
                    }

                }
                else if(sign_type == EN_END_GAME)
                {
                    std::string name;
                    name = net_msg.name();
                    
                    unsigned int game_seq = net_msg.game_seq(), uid = mysql->GetUidByName(name);
                    bool won = net_msg.won();
                    bool ret = server->balance(game_seq, uid, won);
                    if(ret)
                    {
                        ServerLog->info("game %u sccussifully balanced", game_seq);
                    }
                    else 
                    {
                        ServerLog->error("failed to balance game %u", game_seq);
                        continue;
                    }
                }
                else
                {
                    ServerLog->error("unknown request type");
                    continue;
                }
            }
            ServerLog->info("thread routine end on fd: %d", connfd);
        }  
    }
}

bool CServer::sign_in(const std::string & name, const std::string & passwd, msg::Login_info & login_msg)
{
    bool ret = true;;
    LoginRsp result;
    unsigned int uid = mysql->GetUidByName(name);
    
    if(uid == 0)
    {
        result = EN_LOGIN_WRONG_NAME;
        ret = false;
    }
    else
    {
        if(mysql->Mp_name2record[name].passwd == passwd)
        {
            result = EN_LOGIN_SCCUSS;
        }
        else 
        {
            result = EN_LOGIN_WRONG_PASSWD;
            ret = false;
        }
    }
    login_msg.set_result(result);
    return ret;
}


bool CServer::sign_up(std::string & name, std::string & passwd, msg::Login_info & login_msg)
{
    bool ret = true;
    LoginRsp result;
    sql::ResultSet *rss;
    std::ostringstream oss;
    auto stm = mysql->stm;
    if(mysql->GetUidByName(name) != 0)
    {
        result = EN_LOGIN_USER_EXIST;
        ret = false;
    }
    else
    {
        name = "'" + name;
        name += "'";
        passwd = "'" + passwd;
        passwd += "'"; 

        oss.clear();
        oss << "insert into " << TABLENAME << "(" << NAME_FIELD << "," 
        << PASSWD_FIELD << "," << ALL_GAME_FIELD << "," << VECTORY_GAME_FIELD << ") values("
        << name << "," << passwd << "," << 0 << 0; 

        stm->execute(oss.str());

        oss.clear();
        oss << "select uid from user where name='" << name << "'";

        rss = stm->executeQuery(oss.str());
        unsigned int newuid = rss->getUInt("uid");
        SRecord record{newuid, name, passwd, 0, 0};
        mysql->Mp_name2record[name] = record;
        mysql->Mp_uid2record[newuid] = record;
        result = EN_LOGIN_SCCUSS;
    }

    login_msg.set_result(result);
    
    return ret;
}

bool CServer::match(const std::string & name, int connfd, SMatchThreadParam * match_param)
{
    int ret;
    unsigned int seq, uid0, uid1 = mysql->GetUidByName(name);
    std::string response;

    swait_lock->lock();
    if(swait_lock->is_waiting)
    {
        //second arrive as client
        ServerLog->info("waiting");
        swait_lock->is_waiting = !swait_lock->is_waiting;
        net_msg.set_result(EN_NEW_GAME_RSP_CLIENT);
        net_msg.set_ip(swait_lock->ip);
        net_msg.SerializeToString(&response);
        ret = send(connfd, response.c_str(), strlen(response.c_str()), 0);
        if(ret == -1)
        {
            ServerLog->error("Send data error: %d", errno);
            return false;
        }
        uid0 = swait_lock->uid;
        swait_lock->unlock();

        game_seq->lock();
        seq = game_seq->global_seq++;
        game_seq->unlock();

        SMatchInfo * match_info = new SMatchInfo(uid0, uid1);
        match_queue->lock();
        match_queue->Mp_match_infos[seq] = match_info;
        match_queue->unlock();
        ServerLog->info("server_uid: %u, client_uid: %u, game_seq: %u", uid0, uid1, seq);
    }
    else 
    {
        //first arrive as server
        ServerLog->info("not waiting");
        swait_lock->is_waiting = !swait_lock->is_waiting;
        net_msg.set_result(EN_NEW_GAME_RSP_SERVER);
        net_msg.SerializeToString(&response);
        ret = send(connfd, response.c_str(), strlen(response.c_str()), 0);
        if(ret == -1)
        {
            ServerLog->error("Send data error: %d", errno);
            return false;
        }
        swait_lock->uid = uid1;
        swait_lock->ip = Tool::Sockaddr2Str(match_param->server);
        std::string server_ip = swait_lock->ip;

        swait_lock->unlock();
        ServerLog->info("server: %u ready, ip: %s", uid1, server_ip);
    }
    return true;
}

bool CServer::balance(unsigned int seq, unsigned int uid, bool won)
{
    match_queue->lock();
    auto it = match_queue->Mp_match_infos.begin();
    for(; it != match_queue->Mp_match_infos.end(); ++it)
    {
        if(it->first == seq)
        {
            unsigned int suid = uid, cuid = it->second->uid2;
            if(uid != it->second->uid1)
            {
                ServerLog->error("game message don't match, game_seq: %u, server_uid: %u, client_uid: %u, requesting server_uid: %u",
                    seq, it->second->uid1, it->second->uid2, uid);

                return false;
            }

            if(won)
            {
                int s_vectory_game = mysql->GetVectoryGameByUid(suid);
                mysql->SetVectoryGame(suid, s_vectory_game+1);
            }
            else
            {
                int c_vectory_game = mysql->GetVectoryGameByUid(cuid);
                mysql->SetVectoryGame(cuid, c_vectory_game+1);
            }
            int s_all_game = mysql->GetAllGameByUid(suid), c_all_game = mysql->GetAllGameByUid(cuid);
            mysql->SetAllGame(suid, s_all_game+1);
            mysql->SetAllGame(cuid, c_all_game+1);
            delete match_queue->Mp_match_infos[seq];
            match_queue->Mp_match_infos.erase(seq);
            match_queue->unlock();
            return true;
        }
    }
    match_queue->unlock();
    return false;
}

void CServer::Start()
{
    ServerLog->info("server start.......................................");
    
    if(StartTaskThread())
    {
        ServerLog->info("all threads successfully started");
    }
    else 
    {
        ServerLog->error("failed to start threads");
        return ;
    }

    sockaddr_in server_addr;
    int size;
    while(true)
    {
        ServerLog->info("before accept");
        int connfd = accept(listenfd, (sockaddr*)(&server_addr), (socklen_t*)(&size));
        ServerLog->info("new request arrive fd: %d", connfd);
        auto tmp = new SMatchThreadParam{connfd, server_addr};
        task_queue->push(tmp);
    }
}

bool CServer::StartTaskThread()
{
    int thread_num = task_threads.size();
    for(auto i = 0; i < thread_num; ++i)
    {
        if(0 == pthread_create(&task_threads[i], NULL, ThreadRoutine, (void *)this))
        {
            ServerLog->info("thread %u successfully started", task_threads[i]);
        }
        else
        {
            ServerLog->error("the %dth thread failed to start, errno: %d", i, errno);
            return false;
        }
    }
    return true;
}