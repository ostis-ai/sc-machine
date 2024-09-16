/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <algorithm>

#include "GenerationUtils.hpp"

#include "IteratorUtils.hpp"
#include "CommonUtils.hpp"

namespace utils
{
ScAddr GenerationUtils::wrapInOrientedSetBySequenceRelation(
    ScMemoryContext * ms_context,
    ScAddrVector const & addrVector,
    ScType const & setType)
{
  ScAddr set = ms_context->GenerateNode(setType);
  if (addrVector.empty())
    return set;

  ScAddr prevEdge = ms_context->GenerateConnector(ScType::ConstPermPosArc, set, addrVector.at(0));
  ms_context->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::rrel_1, prevEdge);

  for (size_t i = 1; i < addrVector.size(); ++i)
  {
    ScAddr edge = ms_context->GenerateConnector(ScType::ConstPermPosArc, set, addrVector.at(i));
    ScAddr sequenceEdge = ms_context->GenerateConnector(ScType::ConstCommonArc, prevEdge, edge);
    ms_context->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_basic_sequence, sequenceEdge);
    prevEdge = edge;
  }

  return set;
}

ScAddr GenerationUtils::wrapInOrientedSet(
    ScMemoryContext * ms_context,
    ScAddrVector const & addrVector,
    ScType const & setType)
{
  size_t const maxRrelCountExceeded = 10;
  SC_ASSERT(addrVector.size() < maxRrelCountExceeded, ("Not create numerated set with more than 10 elements"));
  SC_UNUSED(maxRrelCountExceeded);

  ScAddr set = ms_context->GenerateNode(setType);
  for (size_t i = 0; i < addrVector.size(); ++i)
  {
    ScAddr edge = ms_context->GenerateConnector(ScType::ConstPermPosArc, set, addrVector.at(i));
    ms_context->GenerateConnector(ScType::ConstPermPosArc, IteratorUtils::getRoleRelation(ms_context, i + 1), edge);
  }

  return set;
}

ScAddr GenerationUtils::wrapInSet(ScMemoryContext * ms_context, ScAddrVector const & addrVector, ScType const & setType)
{
  ScAddr set = ms_context->GenerateNode(setType);
  std::for_each(
      addrVector.begin(),
      addrVector.end(),
      [&ms_context, &set](auto const & element)
      {
        ms_context->GenerateConnector(ScType::ConstPermPosArc, set, element);
      });

  return set;
}

void GenerationUtils::addToSet(ScMemoryContext * ms_context, ScAddr const & set, ScAddrVector const & elements)
{
  for (auto const & element : elements)
  {
    GenerationUtils::addToSet(ms_context, set, element);
  }
}

bool GenerationUtils::addToSet(ScMemoryContext * ms_context, ScAddr const & set, ScAddr const & element)
{
  if (!ms_context->CheckConnector(set, element, ScType::ConstPermPosArc))
  {
    ms_context->GenerateConnector(ScType::ConstPermPosArc, set, element);
    return true;
  }

  return false;
}

bool GenerationUtils::addSetToOutline(ScMemoryContext * ms_context, ScAddr const & set, ScAddr const & outline)
{
  if (!set.IsValid() || !outline.IsValid())
    return false;

  ScIterator3Ptr iterator3 = ms_context->CreateIterator3(set, ScType::ConstPermPosArc, ScType::Unknown);
  while (iterator3->Next())
  {
    ms_context->GenerateConnector(ScType::ConstPermPosArc, outline, iterator3->Get(1));
    ms_context->GenerateConnector(ScType::ConstPermPosArc, outline, iterator3->Get(2));
  }
  return true;
}

bool GenerationUtils::addNodeWithOutRelationToOutline(
    ScMemoryContext * ms_context,
    ScAddr const & node,
    ScAddr const & relation,
    ScAddr const & outline)
{
  if (!node.IsValid() || !relation.IsValid() || !outline.IsValid())
    return false;

  ScIterator5Ptr iterator5 = IteratorUtils::getIterator5(ms_context, node, relation);
  while (iterator5->Next())
  {
    for (int i = 1; i < 5; i++)
      ms_context->GenerateConnector(ScType::ConstPermPosArc, outline, iterator5->Get(i));
  }
  return true;
}

bool GenerationUtils::generateRelationBetween(
    ScMemoryContext * ms_context,
    ScAddr const & start,
    ScAddr const & finish,
    ScAddr const & relation)
{
  bool isSuccess = false;
  bool isRole = CommonUtils::checkType(ms_context, relation, ScType::ConstNodeRole);
  ScType arcType = isRole ? ScType::ConstPermPosArc : ScType::ConstCommonArc;
  ScAddr arc = ms_context->GenerateConnector(arcType, start, finish);
  if (arc.IsValid())
  {
    arc = ms_context->GenerateConnector(ScType::ConstPermPosArc, relation, arc);
    isSuccess = arc.IsValid();
  }
  return isSuccess;
}

}  // namespace utils
