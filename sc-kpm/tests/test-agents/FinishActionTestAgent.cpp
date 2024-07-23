#include "FinishActionTestAgent.hpp"

using namespace scUtilsTestAgents;

ScAddr FinishActionTestAgent::GetActionClass() const
{
  return FinishActionTestAgent::finish_action_test_action;
}

ScResult FinishActionTestAgent::DoProgram(ScActionEvent const &, ScAction & action)
{
  ScTemplate actionDescriptionTemplate;
  actionDescriptionTemplate.Quintuple(
      action, ScType::EdgeDCommonVar, ScType::NodeVarStruct, ScType::EdgeAccessVarPosPerm, scAgentsCommon::CoreKeynodes::nrel_answer);
  ScTemplateGenResult actionDescriptionGenResult;
  m_memoryCtx.HelperGenTemplate(actionDescriptionTemplate, actionDescriptionGenResult);

  return action.FinishSuccessfully();
}
