#pragma once

#include "Client.h"
#include <vector>

class Room
{
    std::vector<Client *> room_clients;
    std::vector<char*> chatLog;

public:
    char* name;
    char* owner;

    void roomMngMsg(char* msg);

    void addClient(Client *cl);

    void removeClient(Client *client);

    void broadcastMsg(const char*,const char*);

    int kickClient(char* );

    void sendChatLog(Client *cl);

    Room(Client *ow, char* );
};