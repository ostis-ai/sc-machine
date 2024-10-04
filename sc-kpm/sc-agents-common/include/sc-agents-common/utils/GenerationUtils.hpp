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

namespace utils
{
class GenerationUtils
{
public:
  static ScAddr wrapInOrientedSetBySequenceRelation(
      ScMemoryContext * ms_context,
      ScAddrVector const & addrVector,
      ScType const & setType = ScType::ConstNode);

  static ScAddr wrapInOrientedSet(
      ScMemoryContext * ms_context,
      ScAddrVector const & addrVector,
      ScType const & setType = ScType::ConstNode);

  static ScAddr wrapInSet(
      ScMemoryContext * ms_context,
      ScAddrVector const & addrVector,
      ScType const & setType = ScType::ConstNode);

  static void addToSet(ScMemoryContext * ms_context, ScAddr const & set, ScAddrVector const & elements);

  static bool addToSet(ScMemoryContext * ms_context, ScAddr const & set, ScAddr const & element);

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
