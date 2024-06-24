#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace scUtilsTestAgents
{

class FinishActionTestAgent : public ScAgent<>
{
public:
  SC_AGENT_BODY(FinishActionTestAgent);

  static ScKeynodeClass const msAgentKeynode;
};

}  // namespace scUtilsTestAgents
