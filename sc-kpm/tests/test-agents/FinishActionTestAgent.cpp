#include "FinishActionTestAgent.hpp"

using namespace scUtilsTestAgents;

sc_result FinishActionTestAgent::OnEvent(ScActionEvent const & event)
{
  ScAction const & action = event.GetAction();

  ScTemplate actionDescriptionTemplate;
  actionDescriptionTemplate.Quintuple(
      action, ScType::EdgeDCommonVar, ScType::NodeVarStruct, ScType::EdgeAccessVarPosPerm, scAgentsCommon::CoreKeynodes::nrel_answer);
  ScTemplateGenResult actionDescriptionGenResult;
  m_memoryCtx.HelperGenTemplate(actionDescriptionTemplate, actionDescriptionGenResult);

  return SC_RESULT_OK;
}
