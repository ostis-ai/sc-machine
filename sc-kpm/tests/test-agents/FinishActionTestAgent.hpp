#pragma once

#include "sc-memory/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace scUtilsTestAgents
{

class FinishActionTestAgent : public ScActionAgent<>
{
public:
  SC_ACTION_AGENT_BODY(FinishActionTestAgent);

  static ScKeynodeClass const msAgentKeynode;

  sc_result OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
};

}  // namespace scUtilsTestAgents
