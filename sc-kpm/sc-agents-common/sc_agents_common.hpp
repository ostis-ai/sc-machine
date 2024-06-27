/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/kpm/sc_module.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

class _SC_EXTERN ScAgentsCommonModule final : public ScModule
{
public:
  SC_MODULE_BODY(ScAgentsCommonModule);

  sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) override;
  sc_result Shutdown(ScMemoryContext * ctx) override;
};
