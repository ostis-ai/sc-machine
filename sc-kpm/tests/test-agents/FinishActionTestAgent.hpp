#pragma once

#include "sc-memory/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace scUtilsTestAgents
{

class FinishActionTestAgent : public ScActionAgent<ScKeynodes::action_initiated>
{
public:
  sc_result OnEvent(ScEventAddOutputEdge const & event) override;
};

}  // namespace scUtilsTestAgents
