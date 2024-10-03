/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_set.hpp"

class ScStructure : public ScSet
{
  friend class ScAgentContext;
  friend class ScTemplateLoader;

protected:
  _SC_EXTERN ScStructure(ScMemoryContext * context, ScAddr const & structureAddr);

  _SC_EXTERN ScStructure(ScMemoryContext & context, ScAddr const & structureAddr);
};
