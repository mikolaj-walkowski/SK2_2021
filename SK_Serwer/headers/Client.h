#pragma once

#include <string>
#include "enums.h"

class Room;

class Client
{
public:
    int fd;
    char* nick;
    Room *currRoom;
    Client(int _fd, char* nick);
    Client(int _fd);
    ~Client();
};