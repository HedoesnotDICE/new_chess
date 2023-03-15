#ifndef CMYSQL_H
#define CMYSQL_H

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/metadata.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/exception.h>
#include <cppconn/warning.h>
#include <log4cpp/Category.hh>

//configure for mysql
#define     DBHOST          "tcp://127.0.0.1:3306"
#define     USER            "zyb"
#define     PASSWORD        "zyb151"
#define     DATABASE        "chess_server"
#define     TABLENAME       "user"
#define     NAME_FIELD      "name"
#define     PASSWD_FIELD    "password"
#define     ALL_GAME_FIELD  "all_game"
#define     VECTORY_GAME_FIELD  "vectory_game"

struct SRecord
{
    unsigned int uid;
    std::string name;
    std::string passwd;
    int all_game;
    int vectory_game;
    SRecord(unsigned puid = 0, std::string pname = "", std::string ppasswd = "", int all = 0, int vectory = 0) 
    { 
        uid = puid;
        name = pname;
        passwd = ppasswd;
        all_game = all;
        vectory_game = vectory;
    }
};
class CMysql
{
public:
    sql::Driver *driver ;
    sql::Connection *conn ;
    sql::Statement *stm;
    std::map<unsigned int, SRecord> Mp_uid2record;
    std::map<std::string, SRecord> Mp_name2record;
    log4cpp::Category * ServerLog; 

public:
    CMysql();
    bool Init(log4cpp::Category *);
    sql::Statement * GetStm();
    unsigned int GetUidByName(const std::string & name);
    int GetAllGameByUid(unsigned int uid);
    int GetVectoryGameByUid(unsigned int uid);
    bool SetAllGame(unsigned int uid, int all_game);
    bool SetVectoryGame(unsigned int uid, int vectory_game);
};

#endif