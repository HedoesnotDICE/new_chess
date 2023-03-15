#include <iostream>
#include "../net_msg.pb.h"
#include "../my_types.h"

int main()
{
    std::string name = "aaa";
    std::string sock_data;
    msg::Login_info net_msg;
    net_msg.set_sign_type(EN_NEW_GAME);
    net_msg.set_name(name);
    net_msg.SerializeToString(&sock_data);

    std::string arrive_data(sock_data.data(), sock_data.size()-1);
    msg::Login_info arrive_msg;
    arrive_msg.ParseFromString(arrive_data);

    std::cout << arrive_msg.SerializeAsString();

    return 0;
}