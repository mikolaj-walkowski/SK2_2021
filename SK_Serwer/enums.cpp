#include "enums.h"

std::map<std::string, protocol_keywords> enum_resolver = {{"msg_size", msg_size}, {"msg", msg}, {"kick", kick}, {"join", join}, {"leave", leave}, {"create", create}};

protocol_keywords resolve_pkw(std::string s)
{
    try
    {
        return enum_resolver.at(s);
    }
    catch (const std::exception &e)
    {
        return bad;
    }
}