#include "Client.h"

Client::Client(int _fd, std::string _ip) : fd(_fd), ip(_ip)
    {
        currRoom = NULL;
        cs = newChain;
    }