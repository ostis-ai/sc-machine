/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_structure.hpp"
#include "sc-memory/sc_memory.hpp"

ScStructure::ScStructure(ScMemoryContext * context, ScAddr const & structureAddr)
  : ScSet(context, structureAddr)
{
}

ScStructure::ScStructure(ScMemoryContext & context, ScAddr const & structureAddr)
  : ScStructure(&context, structureAddr)
{
}
