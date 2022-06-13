/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "SetOperationsUtils.hpp"
#include "CommonUtils.hpp"

namespace utils
{
ScAddr SetOperationsUtils::uniteSets(ScMemoryContext * context, const ScAddrVector & sets, const ScType & resultType)
{
  ScAddr resultSet = context->CreateNode(resultType);

  for (const auto & set : sets)
  {
    ScIterator3Ptr firstIter3 = context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::Unknown);

    while (firstIter3->Next())
    {
      ScAddr element = firstIter3->Get(2);

      if (!context->HelperCheckEdge(resultSet, element, ScType::EdgeAccessConstPosPerm))
      {
        context->CreateEdge(ScType::EdgeAccessConstPosPerm, resultSet, element);
      }
    }
  }

  return resultSet;
}

ScAddr SetOperationsUtils::intersectSets(
    ScMemoryContext * context,
    const ScAddrVector & sets,
    const ScType & resultType)
{
  ScAddr resultSet = context->CreateNode(resultType);

  for (const auto & set : sets)
  {
    ScIterator3Ptr firstIter3 = context->Iterator3(set, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
    while (firstIter3->Next())
    {
      ScAddr element = firstIter3->Get(2);

      bool isCommon = true;

      if (!context->HelperCheckEdge(resultSet, element, ScType::EdgeAccessConstPosPerm))
      {
        for (const auto & otherSet : sets)
        {
          if (otherSet == set)
          {
            continue;
          }

          if (context->HelperCheckEdge(otherSet, element, ScType::EdgeAccessConstPosPerm))
          {
            isCommon = false;
            break;
          }
        }

        if (isCommon)
        {
          context->CreateEdge(ScType::EdgeAccessConstPosPerm, resultSet, element);
        }
      }
    }
  }

  return resultSet;
}

ScAddr SetOperationsUtils::complementSets(
    ScMemoryContext * context,
    const ScAddr & firstSet,
    const ScAddr & secondSet,
    const ScType & resultType)
{
  SC_CHECK_PARAM(firstSet, ("Invalid first set address"));
  SC_CHECK_PARAM(secondSet, ("Invalid second set address"));

  ScAddr resultSet = context->CreateNode(resultType);

  ScIterator3Ptr secondIter3 = context->Iterator3(secondSet, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  while (secondIter3->Next())
  {
    ScAddr element = secondIter3->Get(2);

    if (!context->HelperCheckEdge(firstSet, element, ScType::EdgeAccessConstPosPerm) &&
        !context->HelperCheckEdge(resultSet, element, ScType::EdgeAccessConstPosPerm))
    {
      context->CreateEdge(ScType::EdgeAccessConstPosPerm, resultSet, element);
    }
  }

  return resultSet;
}

bool SetOperationsUtils::compareSets(ScMemoryContext * context, const ScAddr & firstSet, const ScAddr & secondSet)
{
  SC_CHECK_PARAM(firstSet, ("Invalid first set address"));
  SC_CHECK_PARAM(secondSet, ("Invalid second set address"));

  bool isEqual = CommonUtils::getSetPower(context, firstSet) == CommonUtils::getSetPower(context, secondSet);
  if (!isEqual)
  {
    return false;
  }

  ScIterator3Ptr firstIter3 = context->Iterator3(firstSet, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  while (firstIter3->Next())
  {
    ScAddr element = firstIter3->Get(2);

    if (!context->HelperCheckEdge(secondSet, element, ScType::EdgeAccessConstPosPerm))
    {
      return false;
    }
  }

  return true;
}

}  // namespace utils
