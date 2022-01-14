#pragma once

#include "Client.h"
#include <vector>

class Room
{
    std::vector<Client *> room_clients;
    std::vector<std::pair<std::string, std::string>> chatLog;

public:
    std::string name;
    std::string owner;

    void roomMngMsg(std::string msg);

    void addClient(Client *cl);

    void removeClient(Client *client);

    void broadcastMsg(Client *cl, std::string msg);

    int kickClient(std::string ip);

    void sendChatLog(Client *cl);

    Room(Client *ow, std::string _name);
};