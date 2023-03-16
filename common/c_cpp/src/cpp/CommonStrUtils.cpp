#include <wombat/wlock.h>
#include "wombat/CommonStrUtils.h"

namespace Wombat
{

std::string&
CommonStrUtils::ltrim(std::string& s, const char* t)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

std::string&
CommonStrUtils::rtrim(std::string& s, const char* t)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

std::string&
CommonStrUtils::trim(std::string& s, const char* t)
{
    return ltrim(rtrim(s, t), t);
}

std::vector<std::string>
CommonStrUtils::split (const char* input, const char* delim) {
    std::vector<std::string> list;
    std::string delimiter = delim;
    std::string s = input;
    size_t pos;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        list.push_back(trim(token));
        s.erase(0, pos + delimiter.length());
    }
    list.push_back(trim(s));
    return list;
}

}