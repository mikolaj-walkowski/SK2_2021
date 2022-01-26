#include "Room.h"
#include <unistd.h>
#include <string.h>
#include "enums.h"



void Room::roomMngMsg(char* msg)
{
    broadcastMsg("server",msg);
}

void Room::addClient(Client *cl)
{
    char* msg = createBuffer(NULL,2,cl->nick," joined");
    sprintf(msg,"%s joined", cl->nick );
    roomMngMsg(msg);
    free(msg);
    room_clients.push_back(cl);
    sendChatLog(cl);
    printf("\tRoom %s current size: %d\n", name ,room_clients.size());
}

void Room::removeClient(Client *client)
{
    for (auto itr = room_clients.begin(); itr != room_clients.end(); itr++)
    {
        if ((*itr) == client)
        {
            char* tmp =(*itr)->nick; 
            char* msg = createBuffer(NULL,2,tmp," left");
            sprintf(msg,"%s left", tmp);
            roomMngMsg(msg);
            free(msg);
            return;
        }
    }
}

void Room::broadcastMsg(const char* sender,const char* msg)
{
    printf("\tRoom %s brodcasting msg %s from %s\n",name,sender, msg);
    int msgLen;
    char* cmsg = createBuffer(&msgLen,3,sender,msg,"msg  ");
    sprintf(cmsg,"msg %s\n\t%s",sender,msg);
    chatLog.push_back(cmsg);
    for (auto itr : room_clients)
    {
            if (write(itr->fd, cmsg, msgLen) == -1)
            {
                perror("Couldnt write to socket");
            }
    }
}

int Room::kickClient(char* nick)
{
    for (auto itr = room_clients.begin(); itr != room_clients.end(); itr++)
        if ((*itr)->nick == nick)
        {
            if (write((*itr)->fd, "kick", 5) == -1)
            {
                perror("Couldn't write to socket");
                return 1;
            }
            room_clients.erase(itr);
            char* cmsg = createBuffer(NULL,2,"Kicked ", nick);
            sprintf(cmsg, "Kicked %s", nick);
            roomMngMsg(cmsg);
            free(cmsg);
            return 0;
        }
    return 1;
}

void Room::sendChatLog(Client *cl)
{
    for (auto itr : chatLog)
    {
        if (write(cl->fd, itr, strlen(itr)+1) == -1)
        {
            perror("Couldnt write to socket");
        }
    }
}

Room::Room(Client *ow, char* _name) : owner(ow->nick), name(_name)
{
    addClient(ow);
    ow->currRoom = this;
}