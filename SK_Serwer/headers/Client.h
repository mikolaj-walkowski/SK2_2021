#pragma once

#include <string>
#include "enums.h"

class Room;

class Client
{
public:
    int fd;
    client_states cs;
    std::string ip;
    int msg_size = 0;
    Room *currRoom;
    Client(int _fd, std::string _ip);
};