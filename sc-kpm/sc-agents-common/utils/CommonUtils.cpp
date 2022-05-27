/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "CommonUtils.hpp"

#include <sc-memory/sc_stream.hpp>

#include "IteratorUtils.hpp"
#include "GenerationUtils.hpp"
#include "keynodes/coreKeynodes.hpp"

using namespace std;
using namespace scAgentsCommon;

namespace utils
{
bool CommonUtils::checkType(ScMemoryContext * ms_context, const ScAddr & element, ScType scType)
{
  SC_CHECK_PARAM(element, ("Invalid element address"));

  ScType elementType = ms_context->GetElementType(element);
  return (elementType & scType) == scType;
}

int CommonUtils::readInt(ScMemoryContext * ms_context, const ScAddr & scLink)
{
  SC_CHECK_PARAM(scLink, ("Invalid number node address"));

  const ScStreamPtr stream = ms_context->GetLinkContent(scLink);
  if (stream->IsValid())
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

int CommonUtils::readNumber(ScMemoryContext * ms_context, const ScAddr & number)
{
  SC_CHECK_PARAM(number, ("Invalid number node address"));

  ScAddr scLink = IteratorUtils::getAnyByOutRelation(ms_context, number, CoreKeynodes::nrel_idtf);
  return readInt(ms_context, scLink);
}

string CommonUtils::readString(ScMemoryContext * ms_context, const ScAddr & scLink)
{
  return getLinkContent(ms_context, scLink);
}

string CommonUtils::getLinkContent(ScMemoryContext * ms_context, const ScAddr & scLink)
{
  SC_CHECK_PARAM(scLink, ("Invalid link address"));

  string result;
  const ScStreamPtr stream = ms_context->GetLinkContent(scLink);
  if (stream->IsValid())
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

string CommonUtils::getIdtfValue(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & idtfRelation)
{
  return getIdtf(ms_context, node, idtfRelation);
}

string CommonUtils::getIdtf(
    ScMemoryContext * ms_context,
    const ScAddr & node,
    const ScAddr & idtfRelation,
    const ScAddrVector & linkClasses)
{
  string value;
  ScAddr scLink;
  if (linkClasses.empty())
    scLink = IteratorUtils::getAnyByOutRelation(ms_context, node, idtfRelation);
  else
  {
    ScAddrVector scLinks = IteratorUtils::getAllByOutRelation(ms_context, node, idtfRelation);
    for (const ScAddr & link : scLinks)
    {
      scLink = link;
      for (const ScAddr & classAddr : linkClasses)
      {
        if (!ms_context->HelperCheckEdge(classAddr, link, ScType::EdgeAccessConstPosPerm))
        {
          scLink = {};
          break;
        }
      }

      if (scLink.IsValid())
        break;
    }
  }

  if (scLink.IsValid())
    value = CommonUtils::getLinkContent(ms_context, scLink);
  return value;
}

string CommonUtils::getMainIdtf(ScMemoryContext * ms_context, const ScAddr & node, const ScAddrVector & linkClasses)
{
  ScAddr mainIdtfNode = CoreKeynodes::nrel_main_idtf;
  return getIdtf(ms_context, node, mainIdtfNode, linkClasses);
}

void CommonUtils::setIdtf(
    ScMemoryContext * ms_context,
    const ScAddr & node,
    const ScAddr & relation,
    const string & identifier,
    const ScAddrVector & linkClasses)
{
  ScAddr link = ms_context->CreateLink();
  shared_ptr<ScStream> identifierStream = ScStreamConverter::StreamFromString(identifier);
  ms_context->SetLinkContent(link, identifierStream);
  for (ScAddr linkClass : linkClasses)
  {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, linkClass, link);
  }

  utils::GenerationUtils::generateRelationBetween(ms_context, node, link, relation);
}

void CommonUtils::setMainIdtf(
    ScMemoryContext * ms_context,
    const ScAddr & node,
    const string & identifier,
    const ScAddrVector & linkClasses)
{
  ScAddr mainIdtfNode = CoreKeynodes::nrel_main_idtf;
  setIdtf(ms_context, node, mainIdtfNode, identifier, linkClasses);
}

int CommonUtils::getPowerOfSet(ScMemoryContext * ms_context, const ScAddr & set)
{
  return (int)getSetPower(ms_context, set);
}

size_t CommonUtils::getSetPower(ScMemoryContext * ms_context, const ScAddr & set)
{
  SC_CHECK_PARAM(set, ("Invalid set address"));

  int power = 0;
  ScIterator3Ptr iterator3 = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  while (iterator3->Next())
    power++;
  return power;
}

bool CommonUtils::isEmpty(ScMemoryContext * ms_context, const ScAddr & set)
{
  SC_CHECK_PARAM(set, ("Invalid set address"));

  ScIterator3Ptr iterator3 = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  return !iterator3->Next();
}

}  // namespace utils
