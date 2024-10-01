/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "CommonUtils.hpp"

#include "IteratorUtils.hpp"
#include "GenerationUtils.hpp"

namespace utils
{
bool CommonUtils::checkType(ScMemoryContext * ms_context, ScAddr const & element, ScType scType)
{
  SC_CHECK_PARAM(element, "Invalid element address passed to `checkType`");

  ScType elementType = ms_context->GetElementType(element);
  return (elementType & scType) == scType;
}

std::string CommonUtils::getIdtf(
    ScMemoryContext * ms_context,
    ScAddr const & node,
    ScAddr const & idtfRelation,
    ScAddrVector const & linkClasses)
{
  std::string value;
  ScAddr scLink;
  if (linkClasses.empty())
    scLink = IteratorUtils::getAnyByOutRelation(ms_context, node, idtfRelation);
  else
  {
    ScAddrVector scLinks = IteratorUtils::getAllByOutRelation(ms_context, node, idtfRelation);
    for (ScAddr const & link : scLinks)
    {
      scLink = link;
      for (ScAddr const & classAddr : linkClasses)
      {
        if (!ms_context->CheckConnector(classAddr, link, ScType::ConstPermPosArc))
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
    ms_context->GetLinkContent(scLink, value);
  return value;
}

std::string CommonUtils::getMainIdtf(
    ScMemoryContext * ms_context,
    ScAddr const & node,
    ScAddrVector const & linkClasses)
{
  ScAddr mainIdtfNode = ScKeynodes::nrel_main_idtf;
  return getIdtf(ms_context, node, mainIdtfNode, linkClasses);
}

void CommonUtils::setIdtf(
    ScMemoryContext * ms_context,
    ScAddr const & node,
    ScAddr const & relation,
    std::string const & identifier,
    ScAddrVector const & linkClasses)
{
  ScAddr link = ms_context->GenerateLink();
  ScStreamPtr identifierStream = ScStreamConverter::StreamFromString(identifier);
  ms_context->SetLinkContent(link, identifierStream);
  for (ScAddr linkClass : linkClasses)
  {
    ms_context->GenerateConnector(ScType::ConstPermPosArc, linkClass, link);
  }

  utils::GenerationUtils::generateRelationBetween(ms_context, node, link, relation);
}

void CommonUtils::setMainIdtf(
    ScMemoryContext * ms_context,
    ScAddr const & node,
    std::string const & identifier,
    ScAddrVector const & linkClasses)
{
  ScAddr mainIdtfNode = ScKeynodes::nrel_main_idtf;
  setIdtf(ms_context, node, mainIdtfNode, identifier, linkClasses);
}

size_t CommonUtils::getSetPower(ScMemoryContext * ms_context, ScAddr const & set)
{
  SC_CHECK_PARAM(set, "Invalid set address passed to `getSetPower`");

  int power = 0;
  ScIterator3Ptr iterator3 = ms_context->CreateIterator3(set, ScType::ConstPermPosArc, ScType::Unknown);
  while (iterator3->Next())
    power++;
  return power;
}

bool CommonUtils::isEmpty(ScMemoryContext * ms_context, ScAddr const & set)
{
  SC_CHECK_PARAM(set, "Invalid set address to `isEmpty`");

  ScIterator3Ptr iterator3 = ms_context->CreateIterator3(set, ScType::ConstPermPosArc, ScType::Unknown);
  return !iterator3->Next();
}

std::string CommonUtils::getAddrHashString(ScAddr const & scAddr)
{
  return std::to_string(SC_ADDR_LOCAL_TO_INT(scAddr.GetRealAddr()));
}

}  // namespace utils
