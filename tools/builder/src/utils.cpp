#include "utils.h"


void StringUtil::toLowerCase(String &str)
{
    std::transform(
        str.begin(),
        str.end(),
        str.begin(),
        tolower);
}

//-----------------------------------------------------------------------
void StringUtil::toUpperCase(String &str)
{
    std::transform(
        str.begin(),
        str.end(),
        str.begin(),
        toupper);
}

bool StringUtil::startsWith(const String &str, const String &pattern, bool lowerCase)
{
    size_t thisLen = str.length();
    size_t patternLen = pattern.length();
    if (thisLen < patternLen || patternLen == 0)
        return false;

    String startOfThis = str.substr(0, patternLen);
    if (lowerCase)
        StringUtil::toLowerCase(startOfThis);

    return (startOfThis == pattern);
}

bool StringUtil::endsWith(const std::string &str, const std::string &pattern, bool lowerCase)
{
    size_t thisLen = str.length();
    size_t patternLen = pattern.length();
    if (thisLen < patternLen || patternLen == 0)
        return false;

    String endOfThis = str.substr(thisLen - patternLen, patternLen);
    if (lowerCase)
        StringUtil::toLowerCase(endOfThis);

    return (endOfThis == pattern);
}
