/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <sc-memory/cpp/sc_memory.hpp>
#include <sc-memory/cpp/sc_stream.hpp>

#include "CommonUtils.hpp"
#include "IteratorUtils.hpp"
#include "keynodes/coreKeynodes.hpp"

using namespace std;
using namespace scAgentsCommon;

namespace utils
{

bool CommonUtils::checkType(ScMemoryContext * ms_context, ScAddr const & node, ScType scType)
{
  ScType nodeType = ms_context->GetElementType(node);
  return (nodeType & scType) == scType;
}

int CommonUtils::readInt(ScMemoryContext * ms_context, ScAddr const & scLink)
{
  ScStream stream;
  if (ms_context->GetLinkContent(scLink, stream))
  {
    string str;
    if (ScStreamConverter::StreamToString(stream, str))
    {
      stringstream streamString(str);
      int result;
      streamString >> result;
      return result;
    }
  }
  return -1;
}

int CommonUtils::readNumber(ScMemoryContext * ms_context, ScAddr const & number)
{
  ScAddr scLink = IteratorUtils::getFirstByOutRelation(ms_context, number, CoreKeynodes::nrel_idtf);
  return readInt(ms_context, scLink);
}

string CommonUtils::readString(ScMemoryContext * ms_context, ScAddr const & scLink)
{
  string result;
  ScStream stream;
  if (ms_context->GetLinkContent(scLink, stream))
  {
    string str;
    if (ScStreamConverter::StreamToString(stream, str))
    {
      stringstream streamString(str);
      result = streamString.str();
    }
  }
  return result;
}

string CommonUtils::getIdtfValue(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & idtfRelation)
{
  string value;
  ScAddr scLink = IteratorUtils::getFirstByOutRelation(ms_context, node, idtfRelation);
  if (scLink.IsValid())
    value = CommonUtils::readString(ms_context, scLink);
  return value;
}

int CommonUtils::getPowerOfSet(ScMemoryContext * ms_context, ScAddr const & set)
{
  int power = 0;
  ScIterator3Ptr iterator3 = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  while (iterator3->Next())
    power++;
  return power;
}
}
