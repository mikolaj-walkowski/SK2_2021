#include "Room.h"
#include <unistd.h>


void Room::roomMngMsg(std::string msg)
{
    broadcastMsg("server",msg);
}

void Room::addClient(Client *cl)
{
    roomMngMsg(cl->ip + " joined");
    room_clients.push_back(cl);
    sendChatLog(cl);
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
            roomMngMsg(tmp + " left");
            return;
        }
    }
}

void Room::broadcastMsg(std::string ip, std::string msg)
{
    chatLog.push_back(std::make_pair(ip, msg));
    for (auto itr : room_clients)
    {
            std::string cmsg = "msg " + ip + ": " + msg;
            if (write(itr->fd, cmsg.c_str(), cmsg.length()) == -1)
            {
                perror("Couldnt write to socket");
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
            roomMngMsg("Kicked " + ip);
            return 0;
        }
    return 1;
}

void Room::sendChatLog(Client *cl)
{
    for (auto itr : chatLog)
    {
        std::string cmsg = "msg " + itr.first + ": " + itr.second;
        if (write(cl->fd, cmsg.c_str(), cmsg.length()+1) == -1)
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