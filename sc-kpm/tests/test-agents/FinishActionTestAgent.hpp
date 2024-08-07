#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "generated/FinishActionTestAgent.generated.hpp"

namespace scUtilsTestAgents
{

class FinishActionTestAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::action_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

}  // namespace scUtilsTestAgents
