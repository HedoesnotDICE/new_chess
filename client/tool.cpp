
#include "tool.h"

const QString & en2string(uint32_t en_rsp)
{
    static QString response;
    switch (en_rsp) {
    case EN_LOGIN_SCCUSS :
        response = "sccuss";
        break;
    case EN_LOGIN_WRONG_NAME :
        response = "wrong_name";
        break;
    case EN_LOGIN_WRONG_PASSWD :
        response = "wrong_password";
        break;
    case EN_LOGIN_USER_EXIST:
        response = "user_exist";
        break;

    default:
        response = "unknown response type";
    }
    return response;
}
