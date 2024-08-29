/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_structure.hpp"
#include "sc_memory.hpp"

ScStructure::ScStructure(ScMemoryContext * ctx, ScAddr const & structureAddr)
  : ScSet(ctx, structureAddr)
{
}

ScStructure::ScStructure(ScMemoryContext & ctx, ScAddr const & structureAddr)
  : ScStructure(&ctx, structureAddr)
{
}
