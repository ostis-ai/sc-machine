/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_type.hpp>
#include <sc-memory/sc_iterator.hpp>

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace utils
{
class IteratorUtils
{
public:
  static ScAddr getRoleRelation(ScMemoryContext * ms_context, size_t const & index);

  static ScAddr getAnyFromSet(ScMemoryContext * ms_context, ScAddr const & set);

  static ScAddr getNextFromSet(
      ScMemoryContext * ms_context,
      ScAddr const & set,
      ScAddr const & previous,
      ScAddr const & sequenceRelation = scAgentsCommon::CoreKeynodes::nrel_basic_sequence);

  static ScAddrVector getAllWithType(ScMemoryContext * ms_context, ScAddr const & set, ScType scType);

  static ScAddrVector getAllByRelation(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScAddr const & relation,
      bool isBeginNode = true);

  static ScAddrVector getAllByInRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation);

  static ScAddr getAnyByInRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation);

  static ScAddrVector getAllByOutRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation);

  static ScAddr getAnyByOutRelation(ScMemoryContext * ms_context, ScAddr const & node, ScAddr const & relation);

  static ScIterator5Ptr getIterator5(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScAddr const & relation,
      bool nodeIsStart = true);
};

}  // namespace utils
