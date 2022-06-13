/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_utils.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>

namespace utils
{
void StringUtils::ToLowerCase(std::string & str)
{
  std::transform(str.begin(), str.end(), str.begin(), tolower);
}

//-----------------------------------------------------------------------
void StringUtils::ToUpperCase(std::string & str)
{
  std::transform(str.begin(), str.end(), str.begin(), toupper);
}

bool StringUtils::StartsWith(std::string const & str, std::string const & pattern, bool lowerCase)
{
  size_t thisLen = str.length();
  size_t patternLen = pattern.length();
  if (thisLen < patternLen || patternLen == 0)
    return false;

  std::string startOfThis = str.substr(0, patternLen);
  if (lowerCase)
    StringUtils::ToLowerCase(startOfThis);

  return (startOfThis == pattern);
}

bool StringUtils::EndsWith(std::string const & str, std::string const & pattern, bool lowerCase)
{
  size_t thisLen = str.length();
  size_t patternLen = pattern.length();
  if (thisLen < patternLen || patternLen == 0)
    return false;

  std::string endOfThis = str.substr(thisLen - patternLen, patternLen);
  if (lowerCase)
    StringUtils::ToLowerCase(endOfThis);

  return (endOfThis == pattern);
}

std::string StringUtils::GetFileExtension(std::string const & filename)
{
  // get file extension
  std::string path = filename;
  std::replace(path.begin(), path.end(), '\\', '/');
  size_t start = path.find_last_of('/');
  size_t n = path.find('.', start);
  if (n == std::string::npos)
    return {};

  return path.substr(n + 1, std::string::npos);
}

void StringUtils::SplitFilename(std::string const & qualifiedName, std::string & outBasename, std::string & outPath)
{
  std::string path = qualifiedName;
  // Replace \ with / first
  std::replace(path.begin(), path.end(), '\\', '/');
  // split based on final /
  size_t i = path.find_last_of('/');

  if (i == std::string::npos)
  {
    outPath.clear();
    outBasename = qualifiedName;
  }
  else
  {
    outBasename = path.substr(i + 1, path.size() - i - 1);
    outPath = path.substr(0, i + 1);
  }
}

void StringUtils::SplitString(std::string const & str, char delim, std::vector<std::string> & outList)
{
  outList.clear();
  std::istringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delim))
    outList.push_back(item);
}

std::string StringUtils::NormalizeFilePath(std::string const & init, bool makeLowerCase)
{
  const char * bufferSrc = init.c_str();
  int pathLen = (int)init.size();
  int indexSrc = 0;
  int indexDst = 0;
  int metaPathArea = 0;

  char reservedBuf[1024];
  char * bufferDst = reservedBuf;
  bool isDestAllocated = false;
  if (pathLen > 1023)
  {
    // if source path is to long ensure we don't do a buffer overrun by allocating some
    // new memory
    isDestAllocated = true;
    bufferDst = new char[pathLen + 1];
  }

  // The outer loop loops over directories
  while (indexSrc < pathLen)
  {
    if ((bufferSrc[indexSrc] == '\\') || (bufferSrc[indexSrc] == '/'))
    {
      // check if we have a directory delimiter if so skip it (we should already
      // have written such a delimiter by this point
      ++indexSrc;
      continue;
    }
    else
    {
      // check if there is a directory to skip of type ".\"
      if ((bufferSrc[indexSrc] == '.') && ((bufferSrc[indexSrc + 1] == '\\') || (bufferSrc[indexSrc + 1] == '/')))
      {
        indexSrc += 2;
        continue;
      }

      // check if there is a directory to skip of type "..\"
      else if (
          (bufferSrc[indexSrc] == '.') && (bufferSrc[indexSrc + 1] == '.') &&
          ((bufferSrc[indexSrc + 2] == '\\') || (bufferSrc[indexSrc + 2] == '/')))
      {
        if (indexDst > metaPathArea)
        {
          // skip a directory backward in the destination path
          do
          {
            --indexDst;
          } while ((indexDst > metaPathArea) && (bufferDst[indexDst - 1] != '/'));
          indexSrc += 3;
          continue;
        }
        else
        {
          // we are about to write "..\" to the destination buffer
          // ensure we will not remove this in future "skip directories"
          metaPathArea += 3;
        }
      }
    }

    // transfer the current directory name from the source to the destination
    while (indexSrc < pathLen)
    {
      char curChar = bufferSrc[indexSrc];
      if (makeLowerCase)
        curChar = tolower(curChar);
      if ((curChar == '\\') || (curChar == '/'))
        curChar = '/';
      bufferDst[indexDst] = curChar;
      ++indexDst;
      ++indexSrc;
      if (curChar == '/')
        break;
    }
  }
  bufferDst[indexDst] = 0;

  std::string normalized(bufferDst);
  if (isDestAllocated)
  {
    delete[] bufferDst;
  }

  return normalized;
}

std::string StringUtils::ReplaceAll(
    std::string const & source,
    std::string const & replaceWhat,
    std::string const & replaceWithWhat)
{
  std::string result = source;
  std::string::size_type pos = 0;
  while (true)
  {
    pos = result.find(replaceWhat, pos);
    if (pos == std::string::npos)
      break;
    result.replace(pos, replaceWhat.size(), replaceWithWhat);
    pos += replaceWithWhat.size();
  }
  return result;
}

void StringUtils::TrimLeft(std::string & str)
{
  if (str.empty())
    return;

  size_t i = 0;
  while (i < str.size() && std::isspace(str[i]))
    ++i;

  if (i < str.size())
    str = str.substr(i);
  else
    str = "";
}

void StringUtils::TrimRight(std::string & str)
{
  if (str.empty())
    return;

  size_t i = str.size() - 1;
  while (i > 0 && std::isspace(str[i]))
    --i;

  if (i > 0)
    str = str.substr(0, i + 1);
  else
    str = "";
}

void StringUtils::Trim(std::string & str)
{
  TrimLeft(str);
  TrimRight(str);
}

int Random::Int()
{
  return std::rand();
}

}  // namespace utils
