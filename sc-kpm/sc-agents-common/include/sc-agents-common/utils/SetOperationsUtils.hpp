/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

namespace utils
{
class SetOperationsUtils
{
public:
  static ScAddr uniteSets(
      ScMemoryContext * context,
      ScAddrVector const & sets,
      ScType const & resultType = ScType::ConstNode);

  static ScAddr intersectSets(
      ScMemoryContext * context,
      ScAddrVector const & sets,
      ScType const & resultType = ScType::ConstNode);

  static ScAddr complementSets(
      ScMemoryContext * context,
      ScAddr const & firstSet,
      ScAddr const & secondSet,
      ScType const & resultType = ScType::ConstNode);

  static bool compareSets(ScMemoryContext * context, ScAddr const & firstSet, ScAddr const & secondSet);
};

}  // namespace utils
