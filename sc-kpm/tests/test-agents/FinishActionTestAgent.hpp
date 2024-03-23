#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "keynodes/coreKeynodes.hpp"

#include "FinishActionTestAgent.generated.hpp"

namespace scUtilsTestAgents
{

class FinishActionTestAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

}  // namespace scUtilsTestAgents
