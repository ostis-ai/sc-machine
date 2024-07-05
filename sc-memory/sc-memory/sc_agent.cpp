/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent.hpp"
#include "sc-memory/sc_keynodes.hpp"

#include "sc_agent_context.hpp"

#include "sc-memory/utils/sc_log.hpp"

std::list<ScEvent *> ScAgentAbstract::m_events;

ScAgentAbstract::ScAgentAbstract()
  : m_memoryCtx(nullptr)
{
}

ScAgentAbstract::~ScAgentAbstract()
{
  m_memoryCtx.Destroy();
}

sc_result ScAgentAbstract::Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr)
{
  internal::ScKeynodesRegister::m_instance.Register(ctx, initMemoryGeneratedStructureAddr);
  return SC_RESULT_OK;
}

sc_result ScAgentAbstract::Shutdown(ScMemoryContext *)
{
  return SC_RESULT_OK;
}

void ScAgentAbstract::SetContext(ScAddr const & userAddr)
{
  m_memoryCtx = std::move(ScAgentContext(userAddr));
}

ScEvent::DelegateFuncWithUserAddr ScAgentAbstract::GetCallback()
{
  return ScEvent::DelegateFuncWithUserAddr();
};
