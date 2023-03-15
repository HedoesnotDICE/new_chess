#include "user_data.h"
#include "QDebug"

UserData* UserData::ptr = NULL;

UserData::UserData()
{
    //qDebug() << "initialize of user_data";
}

UserData* UserData::GetInstance()
{
    if(ptr == NULL)
    {
        ptr = new UserData;
    }
    return ptr;
}
