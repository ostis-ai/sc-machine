/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <algorithm>

#include "GenerationUtils.hpp"

#include <sc-memory/sc_iterator.hpp>

#include "IteratorUtils.hpp"
#include "CommonUtils.hpp"

using namespace std;

namespace utils
{

ScAddr GenerationUtils::wrapInSet(ScMemoryContext * ms_context, const ScAddrVector & addrVector)
{
  ScAddr set = ms_context->CreateNode(ScType::NodeConst);
  std::for_each(addrVector.begin(), addrVector.end(), [&ms_context, &set](const auto & element)
  {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, set, element);
  });

  return set;
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
}
