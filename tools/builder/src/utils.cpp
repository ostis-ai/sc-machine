/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

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

String StringUtil::getFileExtension(const String &filename)
{
    // get file extension
    size_t n = filename.rfind(".");
    if (n == std::string::npos)
        return String();

    return filename.substr(n + 1, std::string::npos);
}

void StringUtil::splitFilename(const String& qualifiedName, String& outBasename, String& outPath)
{
    String path = qualifiedName;
    // Replace \ with / first
    std::replace( path.begin(), path.end(), '\\', '/' );
    // split based on final /
    size_t i = path.find_last_of('/');

    if (i == String::npos)
    {
        outPath.clear();
        outBasename = qualifiedName;
    }
    else
    {
        outBasename = path.substr(i+1, path.size() - i - 1);
        outPath = path.substr(0, i+1);
    }

}

String StringUtil::normalizeFilePath(const String& init, bool makeLowerCase)
{
    const char* bufferSrc = init.c_str();
    int pathLen = (int)init.size();
    int indexSrc = 0;
    int indexDst = 0;
    int metaPathArea = 0;

    char reservedBuf[1024];
    char* bufferDst = reservedBuf;
    bool isDestAllocated = false;
    if (pathLen > 1023)
    {
        //if source path is to long ensure we don't do a buffer overrun by allocating some
        //new memory
        isDestAllocated = true;
        bufferDst = new char[pathLen + 1];
    }

    //The outer loop loops over directories
    while (indexSrc < pathLen)
    {
        if ((bufferSrc[indexSrc] == '\\') || (bufferSrc[indexSrc] == '/'))
        {
            //check if we have a directory delimiter if so skip it (we should already
            //have written such a delimiter by this point
            ++indexSrc;
            continue;
        }
        else
        {
            //check if there is a directory to skip of type ".\"
            if ((bufferSrc[indexSrc] == '.') &&
                ((bufferSrc[indexSrc + 1] == '\\') || (bufferSrc[indexSrc + 1] == '/')))
            {
                indexSrc += 2;
                continue;
            }

            //check if there is a directory to skip of type "..\"
            else if ((bufferSrc[indexSrc] == '.') && (bufferSrc[indexSrc + 1] == '.') &&
                ((bufferSrc[indexSrc + 2] == '\\') || (bufferSrc[indexSrc + 2] == '/')))
            {
                if (indexDst > metaPathArea)
                {
                    //skip a directory backward in the destination path
                    do {
                        --indexDst;
                    }
                    while ((indexDst > metaPathArea) && (bufferDst[indexDst - 1] != '/'));
                    indexSrc += 3;
                    continue;
                }
                else
                {
                    //we are about to write "..\" to the destination buffer
                    //ensure we will not remove this in future "skip directories"
                    metaPathArea += 3;
                }
            }
        }

        //transfer the current directory name from the source to the destination
        while (indexSrc < pathLen)
        {
            char curChar = bufferSrc[indexSrc];
            if (makeLowerCase) curChar = tolower(curChar);
            if ((curChar == '\\') || (curChar == '/')) curChar = '/';
            bufferDst[indexDst] = curChar;
            ++indexDst;
            ++indexSrc;
            if (curChar == '/') break;
        }
    }
    bufferDst[indexDst] = 0;

    String normalized(bufferDst);
    if (isDestAllocated)
    {
        delete[] bufferDst;
    }

    return normalized;
}

const String StringUtil::replaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat)
{
    String result = source;
    String::size_type pos = 0;
    while(1)
    {
        pos = result.find(replaceWhat,pos);
        if (pos == String::npos) break;
        result.replace(pos, replaceWhat.size(), replaceWithWhat);
        pos += replaceWithWhat.size();
    }
    return result;
}

