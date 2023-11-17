#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace scUtilsTestAgents
{

class FinishActionTestAgent : public ScAgent<ScEvent::Type::AddInputEdge>
{
public:
  SC_AGENT_BODY(ATestAddInputEdge);

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;

  static ScKeynodeClass const msAgentKeynode;
};

}  // namespace scUtilsTestAgents
