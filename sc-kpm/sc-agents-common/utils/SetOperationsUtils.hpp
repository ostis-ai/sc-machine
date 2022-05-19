/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

using namespace std;

namespace utils
{
class SetOperationsUtils
{
public:
  static ScAddr uniteSets(
      ScMemoryContext * context,
      const ScAddrVector & sets,
      const ScType & resultType = ScType::NodeConst);

  static ScAddr intersectSets(
      ScMemoryContext * context,
      const ScAddrVector & sets,
      const ScType & resultType = ScType::NodeConst);

  static ScAddr complementSets(
      ScMemoryContext * context,
      const ScAddr & firstSet,
      const ScAddr & secondSet,
      const ScType & resultType = ScType::NodeConst);

  static bool compareSets(ScMemoryContext * context, const ScAddr & firstSet, const ScAddr & secondSet);
};

}  // namespace utils
