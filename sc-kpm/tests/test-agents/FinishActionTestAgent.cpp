#include "FinishActionTestAgent.hpp"

using namespace scUtilsTestAgents;

sc_result FinishActionTestAgent::OnEvent(ScEventAddOutputEdge const & event)
{
  ScAddr const & actionAddr = event.GetOtherElement();

  ScTemplate actionDescriptionTemplate;
  actionDescriptionTemplate.Quintuple(
      actionAddr, ScType::EdgeDCommonVar, ScType::NodeVarStruct, ScType::EdgeAccessVarPosPerm, scAgentsCommon::CoreKeynodes::nrel_answer);
  ScTemplateGenResult actionDescriptionGenResult;
  m_memoryCtx.HelperGenTemplate(actionDescriptionTemplate, actionDescriptionGenResult);

  return SC_RESULT_OK;
}
