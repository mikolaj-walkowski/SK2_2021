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
    create,
    nick
};

const static struct{
    const char * key;
    protocol_keywords val;
}PKW_converter[]={
    {"msg_size", msg_size},
    {"msg", msg},
    {"kick", kick},
    {"join", join},
    {"leave", leave},
    {"create", create},
    {"nick", nick}
};

protocol_keywords resolve_pkw(const char*);

char* createBuffer(int *,int,...); 