/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "IteratorUtils.hpp"

#include "CommonUtils.hpp"

std::map<size_t, ScAddr> orderRelationsMap;

namespace utils
{
ScAddr IteratorUtils::getRoleRelation(ScMemoryContext * ms_context, size_t const & index)
{
  size_t minRrelCountExceeded = 1;
  if (index < minRrelCountExceeded)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Unable to create role relation with order number " << index << ". Minimum order number is "
                                                            << minRrelCountExceeded);
  SC_UNUSED(minRrelCountExceeded);

  auto relationIter = orderRelationsMap.find(index);
  if (relationIter == orderRelationsMap.end())
  {
    ScAddr relation =
        ms_context->ResolveElementSystemIdentifier("rrel_" + std::to_string(index), ScType::ConstNodeRole);
    orderRelationsMap.insert({index, relation});
    return relation;
  }
  // @todo: Implement common memory for tests with caching
  // return relationIter->second;
  return ms_context->ResolveElementSystemIdentifier("rrel_" + std::to_string(index), ScType::ConstNodeRole);
}

ScAddr IteratorUtils::getAnyFromSet(ScMemoryContext * ms_context, ScAddr const & set)
{
  SC_CHECK_PARAM(set, "Invalid set address passed to `getAnyFromSet`");

  ScIterator3Ptr iterator3 = ms_context->CreateIterator3(set, ScType::ConstPermPosArc, ScType::Unknown);
  if (iterator3->Next())
  {
    return iterator3->Get(2);
  }
  return {};
}

ScAddr IteratorUtils::getNextFromSet(
    ScMemoryContext * ms_context,
    ScAddr const & set,
    ScAddr const & previous,
    ScAddr const & sequenceRelation)
{
  SC_CHECK_PARAM(set, "Invalid set address passed to `getNextFromSet`");
  SC_CHECK_PARAM(previous, "Invalid previous element address passed to `getNextFromSet`");
  SC_CHECK_PARAM(sequenceRelation, "Invalid sequence relation address passed to `getNextFromSet`");

  ScAddr nextElement;
  ScIterator3Ptr const & previousElementIterator = ms_context->CreateIterator3(set, ScType::ConstPermPosArc, previous);
  if (previousElementIterator->Next())
  {
    ScAddr const & previousElementEdge = previousElementIterator->Get(1);
    ScAddr const & nextElementEdge =
        IteratorUtils::getAnyByOutRelation(ms_context, previousElementEdge, sequenceRelation);
    if (nextElementEdge.IsValid())
    {
      nextElement = ms_context->GetArcTargetElement(nextElementEdge);
    }
  }

  return nextElement;
}

ScAddrVector IteratorUtils::getAllWithType(ScMemoryContext * ms_context, ScAddr const & set, ScType scType)
{
  SC_CHECK_PARAM(set, "Invalid set address passed to `getAllWithType`");

  ScAddrVector elementList;
  ScIterator3Ptr iterator3 = ms_context->CreateIterator3(set, ScType::ConstPermPosArc, scType);
  while (iterator3->Next())
  {
    elementList.push_back(iterator3->Get(2));
  }
  return elementList;
}

ScAddrVector IteratorUtils::getAllByRelation(
    ScMemoryContext * ms_context,
    ScAddr const & node,
    ScAddr const & relation,
    bool isBeginNode)
{
  SC_CHECK_PARAM(node, "Invalid node address passed to `getAllByRelation`");
  SC_CHECK_PARAM(relation, "Invalid relation address passed to `getAllByRelation`");

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
    ScAddr const & node,
    ScAddr const & relation)
{
  return getAllByRelation(ms_context, node, relation, false);
}

ScAddr IteratorUtils::getAnyByInRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation)
{
  SC_CHECK_PARAM(node, "Invalid node address passed to `getAnyByInRelation`");
  SC_CHECK_PARAM(relation, "Invalid relation address passed to `getAnyByInRelation`");

  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context, node, relation, false);
  if (iterator5->Next())
  {
    return iterator5->Get(0);
  }
  return {};
}

ScAddrVector IteratorUtils::getAllByOutRelation(
    ScMemoryContext * ms_context,
    ScAddr const & node,
    ScAddr const & relation)
{
  return getAllByRelation(ms_context, node, relation, true);
}

ScAddr IteratorUtils::getAnyByOutRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation)
{
  SC_CHECK_PARAM(node, "Invalid node address passed to `getAnyByOutRelation`");
  SC_CHECK_PARAM(relation, "Invalid relation address passed to `getAnyByOutRelation`");

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
  SC_CHECK_PARAM(node, "Invalid node address passed to `getIterator5`");
  SC_CHECK_PARAM(relation, "Invalid relation address passed to `getIterator5`");

  ScIterator5Ptr iterator5;
  if (isBeginNode)
  {
    iterator5 = ms_context->CreateIterator5(node, ScType::Unknown, ScType::Unknown, ScType::Unknown, relation);
  }
  else
  {
    iterator5 = ms_context->CreateIterator5(ScType::Unknown, ScType::Unknown, node, ScType::Unknown, relation);
  }
  return iterator5;
}

}  // namespace utils
