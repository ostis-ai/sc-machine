#pragma once

#include "sc-memory/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace scUtilsTestAgents
{

class FinishActionTestAgent : public ScActionAgent
{
public:
  sc_result OnEvent(ScActionEvent const & event) override;
};

}  // namespace scUtilsTestAgents
