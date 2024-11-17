/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_utils.hpp"

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

std::string StringUtils::GetFileExtension(std::string const & filePath)
{
  size_t start = filePath.find_last_of('/');
  size_t n = filePath.find('.', start == std::string::npos ? 0 : start);
  if (n == std::string::npos)
    return {};

  return filePath.substr(n + 1, std::string::npos);
}

void StringUtils::SplitString(std::string const & str, char delim, std::vector<std::string> & outList)
{
  outList.clear();
  std::istringstream ss(str);
  std::string item;
  while (std::getline(ss, item, delim))
    outList.push_back(item);
}

std::string StringUtils::ReplaceAll(
    std::string const & source,
    std::string const & replaceWhat,
    std::string const & replaceWithWhat)
{
  std::string result = source;
  std::string::size_type pos = 0;
  while ((pos = result.find(replaceWhat, pos)) != std::string::npos)
  {
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
