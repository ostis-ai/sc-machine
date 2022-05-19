/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_iterator.hpp>

using namespace std;

namespace utils
{
class GenerationUtils
{
public:
  static ScAddr wrapInOrientedSetBySequenceRelation(
      ScMemoryContext * ms_context,
      const ScAddrVector & addrVector,
      const ScType & setType = ScType::NodeConst);

  static ScAddr wrapInOrientedSet(
      ScMemoryContext * ms_context,
      const ScAddrVector & addrVector,
      const ScType & setType = ScType::NodeConst);

  static ScAddr wrapInSet(
      ScMemoryContext * ms_context,
      const ScAddrVector & addrVector,
      const ScType & setType = ScType::NodeConst);

  static void addToSet(ScMemoryContext * ms_context, const ScAddr & set, const ScAddrVector & elements);

  static bool addToSet(ScMemoryContext * ms_context, const ScAddr & set, const ScAddr & element);

  static bool addSetToOutline(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & outline);

  static bool addNodeWithOutRelationToOutline(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScAddr const & relation,
      ScAddr const & outline);

  static bool generateRelationBetween(
      ScMemoryContext * ms_context,
      ScAddr const & start,
      ScAddr const & finish,
      ScAddr const & relation);
};
}  // namespace utils
