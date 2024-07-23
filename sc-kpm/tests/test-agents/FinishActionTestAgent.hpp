#pragma once

#include "sc-memory/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace scUtilsTestAgents
{

class FinishActionTestAgent : public ScActionAgent
{
public:
  static inline ScKeynode const finish_action_test_action{"finish_action_test_action", ScType::NodeConstClass};

  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;
};

}  // namespace scUtilsTestAgents
