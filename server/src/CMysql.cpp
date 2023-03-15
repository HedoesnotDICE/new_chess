#include "CMysql.h"

CMysql::CMysql()
{
    
}

bool CMysql::Init(log4cpp::Category * log)
{
    ServerLog = log;
    try
    {
        //连接数据库
        driver = get_driver_instance();
        conn = driver->connect(DBHOST, USER, PASSWORD);
        if(!conn->isValid())
        {
            ServerLog->error("failed to connect to mysql");
            return false;
        }

        stm = conn->createStatement();
        if(stm == NULL) 
        {
            ServerLog->error("failed to createstatement");
            return false;
        }

        stm->execute("use " DATABASE);
        // if(stm->execute("use " DATABASE) == false)
        // {
        //     ServerLog->error("failed to change database");
        //     return false;
        // }

        sql::ResultSet *rss;
        rss = stm->executeQuery("SELECT * FROM user");
        while (rss->next())
        {
            unsigned uid = rss->getUInt("uid");
            std::string name = rss->getString("name");
            std::string passwd = rss->getString("password");
            int all_game = rss->getInt("all_game");
            int vectory_game = rss->getInt("vectory_game");
            SRecord record{uid, name, passwd, all_game};
            Mp_uid2record[uid] = record;
            Mp_name2record[name] = record;
        }

        ServerLog->info("successfully connected to mysql, number of record: %d", Mp_name2record.size());
        return true;        
    }
    catch (const sql::SQLException &sqle)
    {
        ServerLog->error("sql errcode:%d,state:%s,what:%s", 
            sqle.getErrorCode(), sqle.getSQLState(), sqle.what());

        return false;
    }
}

sql::Statement * CMysql::GetStm()
{
    return stm;
}

unsigned int CMysql::GetUidByName(const std::string & name)
{
    if(Mp_name2record[name].name != name)
    {
        Mp_name2record.erase(name);
        return 0;
    }
    return Mp_name2record[name].uid;
}

int CMysql::GetAllGameByUid(unsigned int uid)
{
    if(uid == 0) return -1;

    if(Mp_uid2record[uid].uid == 0)
    {
        Mp_uid2record.erase(uid);
        return -1;
    }
    return Mp_uid2record[uid].all_game;
}

int CMysql::GetVectoryGameByUid(unsigned int uid)
{
    if(uid == 0) return -1;

    if(Mp_uid2record[uid].uid == 0)
    {
        Mp_uid2record.erase(uid);
        return -1;
    }
    return Mp_uid2record[uid].vectory_game;
}

bool CMysql::SetAllGame(unsigned int uid, int all_game)
{
    std::ostringstream oss;
    oss << "update user set all_game=" << all_game << " where uid=" << uid;
    stm->execute(oss.str());
    Mp_uid2record[uid].all_game = all_game;
    return true;
}

bool CMysql::SetVectoryGame(unsigned int uid, int vectory_game)
{
    std::ostringstream oss;
    oss << "update user set vectory_game=" << vectory_game << " where uid=" << uid;
    stm->execute(oss.str());
    Mp_uid2record[uid].vectory_game = vectory_game;
    return true;
}
