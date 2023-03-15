#ifndef USER_DATA_H
#define USER_DATA_H

#include <QString>
class UserData
{
     UserData();
public:
     static UserData * ptr;
     static UserData * GetInstance();
     QString name;
     int all_game;
     int vectory_game;
     uint32_t game_seq;
};

#endif // USER_DATA_H
