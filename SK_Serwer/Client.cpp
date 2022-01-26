#include "Client.h"

Client::Client(int _fd, char* _nick) : fd(_fd), nick(_nick)
    {
        currRoom = NULL;
        
    }

Client::Client(int _fd):fd(_fd){
    currRoom = NULL;
    nick = NULL;
}

Client::~Client(){
    free(nick);
}