#include "FinishActionTestAgent.hpp"

using namespace scUtilsTestAgents;

SC_ACTION_AGENT_IMPLEMENTATION(FinishActionTestAgent)
{
  ScTemplate actionDescriptionTemplate;
  actionDescriptionTemplate.Quintuple(
      actionAddr, ScType::EdgeDCommonVar, ScType::NodeVarStruct, ScType::EdgeAccessVarPosPerm, scAgentsCommon::CoreKeynodes::nrel_answer);
  ScTemplateGenResult actionDescriptionGenResult;
  m_memoryCtx.HelperGenTemplate(actionDescriptionTemplate, actionDescriptionGenResult);

  return SC_RESULT_OK;
}
