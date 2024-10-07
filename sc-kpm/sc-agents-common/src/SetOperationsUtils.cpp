/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-agents-common/utils/SetOperationsUtils.hpp"

#include "sc-agents-common/utils/CommonUtils.hpp"

namespace utils
{
ScAddr SetOperationsUtils::uniteSets(ScMemoryContext * context, ScAddrVector const & sets, ScType const & resultType)
{
  ScAddr resultSet = context->GenerateNode(resultType);

  for (auto const & set : sets)
  {
    ScIterator3Ptr firstIter3 = context->CreateIterator3(set, ScType::ConstPermPosArc, ScType::Unknown);

    while (firstIter3->Next())
    {
      ScAddr element = firstIter3->Get(2);

      if (!context->CheckConnector(resultSet, element, ScType::ConstPermPosArc))
      {
        context->GenerateConnector(ScType::ConstPermPosArc, resultSet, element);
      }
    }
  }

  return resultSet;
}

ScAddr SetOperationsUtils::intersectSets(
    ScMemoryContext * context,
    ScAddrVector const & sets,
    ScType const & resultType)
{
  ScAddr resultSet = context->GenerateNode(resultType);

  for (auto const & set : sets)
  {
    ScIterator3Ptr firstIter3 = context->CreateIterator3(set, ScType::ConstPermPosArc, ScType::Unknown);
    while (firstIter3->Next())
    {
      ScAddr element = firstIter3->Get(2);

      bool isCommon = true;

      if (!context->CheckConnector(resultSet, element, ScType::ConstPermPosArc))
      {
        for (auto const & otherSet : sets)
        {
          if (otherSet == set)
          {
            continue;
          }

          if (context->CheckConnector(otherSet, element, ScType::ConstPermPosArc))
          {
            isCommon = false;
            break;
          }
        }

        if (isCommon)
        {
          context->GenerateConnector(ScType::ConstPermPosArc, resultSet, element);
        }
      }
    }
  }

  return resultSet;
}

ScAddr SetOperationsUtils::complementSets(
    ScMemoryContext * context,
    ScAddr const & firstSet,
    ScAddr const & secondSet,
    ScType const & resultType)
{
  SC_CHECK_PARAM(firstSet, "Invalid first set address passed to `compareSets`");
  SC_CHECK_PARAM(secondSet, "Invalid second set address passed to `compareSets`");

  ScAddr resultSet = context->GenerateNode(resultType);

  ScIterator3Ptr secondIter3 = context->CreateIterator3(secondSet, ScType::ConstPermPosArc, ScType::Unknown);
  while (secondIter3->Next())
  {
    ScAddr element = secondIter3->Get(2);

    if (!context->CheckConnector(firstSet, element, ScType::ConstPermPosArc)
        && !context->CheckConnector(resultSet, element, ScType::ConstPermPosArc))
    {
      context->GenerateConnector(ScType::ConstPermPosArc, resultSet, element);
    }
  }

  return resultSet;
}

bool SetOperationsUtils::compareSets(ScMemoryContext * context, ScAddr const & firstSet, ScAddr const & secondSet)
{
  SC_CHECK_PARAM(firstSet, "Invalid first set address passed to `compareSets`");
  SC_CHECK_PARAM(secondSet, "Invalid second set address passed to `compareSets`");

  bool isEqual = CommonUtils::getSetPower(context, firstSet) == CommonUtils::getSetPower(context, secondSet);
  if (!isEqual)
  {
    return false;
  }

  ScIterator3Ptr firstIter3 = context->CreateIterator3(firstSet, ScType::ConstPermPosArc, ScType::Unknown);
  while (firstIter3->Next())
  {
    ScAddr element = firstIter3->Get(2);

    if (!context->CheckConnector(secondSet, element, ScType::ConstPermPosArc))
    {
      return false;
    }
  }

  return true;
}

}  // namespace utils
