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

using namespace std;

namespace utils
{
class IteratorUtils
{
public:
  static ScAddr getRoleRelation(ScMemoryContext * ms_context, size_t const & index);

  SC_DEPRECATED(0.6.0, "Use IteratorUtils::getAnyFromSet(ScMemoryContext * ms_context, const ScAddr & set) instead of.")
  static ScAddr getFirstFromSet(ScMemoryContext * ms_context, const ScAddr & set, bool getStrictlyFirst = false);

  static ScAddr getAnyFromSet(ScMemoryContext * ms_context, const ScAddr & set);

  static ScAddr getNextFromSet(
      ScMemoryContext * ms_context,
      const ScAddr & set,
      const ScAddr & previous,
      const ScAddr & sequenceRelation = scAgentsCommon::CoreKeynodes::nrel_basic_sequence);

  static ScAddrVector getAllWithType(ScMemoryContext * ms_context, const ScAddr & set, ScType scType);

  static ScAddrVector getAllByRelation(
      ScMemoryContext * ms_context,
      const ScAddr & node,
      const ScAddr & relation,
      bool isBeginNode = true);

  static ScAddrVector getAllByInRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation);

  SC_DEPRECATED(
      0.6.0,
      "Use IteratorUtils::getAnyByInRelation"
      "(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation) instead of.")
  static ScAddr getFirstByInRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation);

  static ScAddr getAnyByInRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation);

  SC_DEPRECATED(
      0.6.0,
      "Use IteratorUtils::getAnyByOutRelation"
      "(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation) instead of.")
  static ScAddr getFirstByOutRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation);

  static ScAddrVector getAllByOutRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation);

  static ScAddr getAnyByOutRelation(ScMemoryContext * ms_context, const ScAddr & node, const ScAddr & relation);

  static ScIterator5Ptr getIterator5(
      ScMemoryContext * ms_context,
      const ScAddr & node,
      const ScAddr & relation,
      bool nodeIsStart = true);
};

}  // namespace utils
