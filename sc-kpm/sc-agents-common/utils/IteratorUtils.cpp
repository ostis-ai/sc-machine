/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "CommonUtils.hpp"

#include <sc-memory/sc_iterator.hpp>

#include "keynodes/coreKeynodes.hpp"
#include "IteratorUtils.hpp"

using namespace std;
using namespace scAgentsCommon;

std::map<size_t, ScAddr> orderRelationsMap;

namespace utils
{
ScAddr IteratorUtils::getRoleRelation(ScMemoryContext * ms_context, const size_t & index)
{
  size_t minRrelCountExceeded = 1;
  SC_ASSERT(index >= minRrelCountExceeded, ());

  auto relationIter = orderRelationsMap.find(index);
  if (relationIter == orderRelationsMap.end())
  {
    ScAddr relation = ms_context->HelperResolveSystemIdtf("rrel_" + to_string(index), ScType::NodeConstRole);
    orderRelationsMap.insert({index, relation});
    return relation;
  }
  // @todo: Implement common memory for tests with caching
  // return relationIter->second;
  return ms_context->HelperResolveSystemIdtf("rrel_" + to_string(index), ScType::NodeConstRole);
}

ScAddr IteratorUtils::getFirstFromSet(ScMemoryContext * ms_context, const ScAddr & set, bool getStrictlyFirst)
{
  if (getStrictlyFirst)
  {
    return getAnyByOutRelation(ms_context, set, CoreKeynodes::rrel_1);
  }
  else
  {
    return getAnyFromSet(ms_context, set);
  }
}

ScAddr IteratorUtils::getAnyFromSet(ScMemoryContext * ms_context, const ScAddr & set)
{
  SC_CHECK_PARAM(set, ("Invalid set address"));

  ScIterator3Ptr iterator3 = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  if (iterator3->Next())
  {
    return iterator3->Get(2);
  }
  return {};
}

ScAddr IteratorUtils::getNextFromSet(
    ScMemoryContext * ms_context,
    const ScAddr & set,
    const ScAddr & previous,
    const ScAddr & sequenceRelation)
{
  SC_CHECK_PARAM(set, ("Invalid set address"));
  SC_CHECK_PARAM(previous, ("Invalid previous element address"));
  SC_CHECK_PARAM(sequenceRelation, ("Invalid sequence relation address"));

  std::string const NEXT_ELEMENT_ALIAS = "_next_element";
  std::string const NEXT_ELEMENT_ACCESS_ARC_ALIAS = "_next_element_access_arc";
  std::string const PREVIOUS_ELEMENT_ACCESS_ARC_ALIAS = "_previous_element_access_arc";

  ScTemplate scTemplate;
  scTemplate.Triple(set, ScType::EdgeAccessVarPosPerm >> PREVIOUS_ELEMENT_ACCESS_ARC_ALIAS, previous);
  scTemplate.Triple(
      set, ScType::EdgeAccessVarPosPerm >> NEXT_ELEMENT_ACCESS_ARC_ALIAS, ScType::Unknown >> NEXT_ELEMENT_ALIAS);
  scTemplate.TripleWithRelation(
      PREVIOUS_ELEMENT_ACCESS_ARC_ALIAS,
      ScType::EdgeDCommonVar,
      NEXT_ELEMENT_ACCESS_ARC_ALIAS,
      ScType::EdgeAccessVarPosPerm,
      sequenceRelation);
  ScTemplateSearchResult searchResult;
  ms_context->HelperSearchTemplate(scTemplate, searchResult);

  if (searchResult.Size() == 1)
  {
    return searchResult[0][NEXT_ELEMENT_ALIAS];
  }
  return {};
}

ScAddrVector IteratorUtils::getAllWithType(ScMemoryContext * ms_context, const ScAddr & set, ScType scType)
{
  SC_CHECK_PARAM(set, ("Invalid set address"));

  ScAddrVector elementList;
  ScIterator3Ptr iterator3 = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, scType);
  while (iterator3->Next())
  {
    elementList.push_back(iterator3->Get(2));
  }
  return elementList;
}

ScAddrVector IteratorUtils::getAllByRelation(
    ScMemoryContext * ms_context,
    const ScAddr & node,
    const ScAddr & relation,
    bool isBeginNode)
{
  SC_CHECK_PARAM(node, ("Invalid node address"));
  SC_CHECK_PARAM(relation, ("Invalid relation address"));

  ScAddrVector elementList;
  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context, node, relation, isBeginNode);
  while (iterator5->Next())
  {
    elementList.push_back(iterator5->Get(isBeginNode ? 2 : 0));
  }
  return elementList;
}

ScAddrVector IteratorUtils::getAllByInRelation(
    ScMemoryContext * ms_context,
    const ScAddr & node,
    const ScAddr & relation)
{
  return getAllByRelation(ms_context, node, relation, false);
}

ScAddr IteratorUtils::getFirstByInRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation)
{
  return getAnyByInRelation(ms_context, node, relation);
}

ScAddr IteratorUtils::getAnyByInRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation)
{
  SC_CHECK_PARAM(node, ("Invalid node address"));
  SC_CHECK_PARAM(relation, ("Invalid relation address"));

  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context, node, relation, false);
  if (iterator5->Next())
  {
    return iterator5->Get(0);
  }
  return {};
}

ScAddr IteratorUtils::getFirstByOutRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation)
{
  return getAnyByOutRelation(ms_context, node, relation);
}

ScAddrVector IteratorUtils::getAllByOutRelation(
    ScMemoryContext * ms_context,
    const ScAddr & node,
    const ScAddr & relation)
{
  return getAllByRelation(ms_context, node, relation, true);
}

ScAddr IteratorUtils::getAnyByOutRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation)
{
  SC_CHECK_PARAM(node, ("Invalid node address"));
  SC_CHECK_PARAM(relation, ("Invalid relation address"));

  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context, node, relation);
  if (iterator5->Next())
  {
    return iterator5->Get(2);
  }
  return {};
}

ScIterator5Ptr IteratorUtils::getIterator5(
    ScMemoryContext * ms_context,
    ScAddr const & node,
    ScAddr const & relation,
    bool const isBeginNode)
{
  SC_CHECK_PARAM(node, ("Invalid node address"));
  SC_CHECK_PARAM(relation, ("Invalid relation address"));

  ScIterator5Ptr iterator5;
  if (isBeginNode)
  {
    iterator5 = ms_context->Iterator5(node, ScType::Unknown, ScType::Unknown, ScType::Unknown, relation);
  }
  else
  {
    iterator5 = ms_context->Iterator5(ScType::Unknown, ScType::Unknown, node, ScType::Unknown, relation);
  }
  return iterator5;
}

}  // namespace utils
