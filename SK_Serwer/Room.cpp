#include "Room.h"
#include <unistd.h>


void Room::roomMngMsg(std::string msg)
{
    chatLog.push_back(make_pair("serwer", msg));
    for (auto itr : room_clients)
    {
        std::string cmsg = "Serwer message: " + msg;
        if (write(itr->fd, cmsg.c_str(), cmsg.length()) == -1)
        {
            perror("Couldnt write to socket");
        }
    }
}

void Room::addClient(Client *cl)
{
    room_clients.push_back(cl);
    roomMngMsg(cl->ip + " joined\n");
    printf(("Room " + name + " current size: %d\n").c_str(), room_clients.size());
}

void Room::removeClient(Client *client)
{
    for (auto itr = room_clients.begin(); itr != room_clients.end(); itr++)
    {
        if ((*itr) == client)
        {
            std::string tmp =(*itr)->ip; 
            room_clients.erase(itr);
            roomMngMsg(tmp + " left\n");
            return;
        }
    }
}

void Room::broadcastMsg(Client *cl, std::string msg)
{
    chatLog.push_back(std::make_pair(cl->ip, msg));
    for (auto itr : room_clients)
    {
        if (cl != itr)
        {
            std::string cmsg = "msg " + cl->ip + ": " + msg;
            if (write(itr->fd, cmsg.c_str(), cmsg.length()) == -1)
            {
                perror("Couldnt write to socket");
            }
        }
    }
}

int Room::kickClient(std::string ip)
{
    for (auto itr = room_clients.begin(); itr != room_clients.end(); itr++)
        if ((*itr)->ip == ip)
        {
            if (write((*itr)->fd, "kick", 5) == -1)
            {
                perror("Couldnt write to socket");
                return 1;
            }
            room_clients.erase(itr);
            roomMngMsg("Kicked " + (*itr)->ip + "\n");
            return 0;
        }
    return 1;
}

void Room::sendChatLog(Client *cl)
{
    for (auto itr : chatLog)
    {
        std::string cmsg = "msg " + itr.first + ": " + itr.second;
        if (write(cl->fd, cmsg.c_str(), cmsg.length()) == -1)
        {
            perror("Couldnt write to socket");
        }
    }
}

Room::Room(Client *ow, std::string _name) : owner(ow->ip), name(_name)
{
    addClient(ow);
    ow->currRoom = this;
}