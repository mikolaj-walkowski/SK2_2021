#pragma once

#include <map>

enum client_states
{
    recMsg,
    newChain
};
enum protocol_keywords
{
    bad,
    msg_size,
    msg,
    kick,
    join,
    leave,
    create
};

protocol_keywords resolve_pkw(std::string s);