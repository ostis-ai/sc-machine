/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <algorithm>

#include "GenerationUtils.hpp"

#include <sc-memory/sc_iterator.hpp>

#include "keynodes/coreKeynodes.hpp"
#include "IteratorUtils.hpp"
#include "CommonUtils.hpp"

using namespace scAgentsCommon;
using namespace std;

namespace utils
{
ScAddr GenerationUtils::wrapInOrientedSetBySequenceRelation(
    ScMemoryContext * ms_context,
    const ScAddrVector & addrVector,
    const ScType & setType)
{
  ScAddr set = ms_context->CreateNode(setType);
  if (addrVector.empty())
    return set;

  ScAddr prevEdge = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, set, addrVector.at(0));
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::rrel_1, prevEdge);

  for (size_t i = 1; i < addrVector.size(); ++i)
  {
    ScAddr edge = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, set, addrVector.at(i));
    ScAddr sequenceEdge = ms_context->CreateEdge(ScType::EdgeDCommonConst, prevEdge, edge);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::nrel_basic_sequence, sequenceEdge);
    prevEdge = edge;
  }

  return set;
}

ScAddr GenerationUtils::wrapInOrientedSet(
    ScMemoryContext * ms_context,
    const ScAddrVector & addrVector,
    const ScType & setType)
{
  const size_t maxRrelCountExceeded = 10;
  SC_ASSERT(addrVector.size() < maxRrelCountExceeded, ());

  ScAddr set = ms_context->CreateNode(setType);
  for (size_t i = 0; i < addrVector.size(); ++i)
  {
    ScAddr edge = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, set, addrVector.at(i));
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, IteratorUtils::getRoleRelation(ms_context, i + 1), edge);
  }

  return set;
}

ScAddr GenerationUtils::wrapInSet(ScMemoryContext * ms_context, const ScAddrVector & addrVector, const ScType & setType)
{
  ScAddr set = ms_context->CreateNode(setType);
  std::for_each(addrVector.begin(), addrVector.end(), [&ms_context, &set](const auto & element) {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, set, element);
  });

  return set;
}

void GenerationUtils::addToSet(ScMemoryContext * ms_context, const ScAddr & set, const ScAddrVector & elements)
{
  for (auto const & element : elements)
  {
    GenerationUtils::addToSet(ms_context, set, element);
  }
}

bool GenerationUtils::addToSet(ScMemoryContext * ms_context, const ScAddr & set, const ScAddr & element)
{
  if (!ms_context->HelperCheckEdge(set, element, ScType::EdgeAccessConstPosPerm))
  {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, set, element);
    return true;
  }

  return false;
}

bool GenerationUtils::addSetToOutline(ScMemoryContext * ms_context, ScAddr const & set, ScAddr const & outline)
{
  if (!set.IsValid() || !outline.IsValid())
    return false;

  ScIterator3Ptr iterator3 = ms_context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  while (iterator3->Next())
  {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, outline, iterator3->Get(1));
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, outline, iterator3->Get(2));
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
      ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, outline, iterator5->Get(i));
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
  bool isRole = CommonUtils::checkType(ms_context, relation, ScType::NodeConstRole);
  ScType arcType = isRole ? ScType::EdgeAccessConstPosPerm : ScType::EdgeDCommonConst;
  ScAddr arc = ms_context->CreateEdge(arcType, start, finish);
  if (arc.IsValid())
  {
    arc = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, relation, arc);
    isSuccess = arc.IsValid();
  }
  return isSuccess;
}

}  // namespace utils
