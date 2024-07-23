/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "AgentUtils.hpp"

#include <sc-memory/sc_wait.hpp>

#include "IteratorUtils.hpp"
#include "GenerationUtils.hpp"

using namespace scAgentsCommon;

namespace utils
{
sc_uint32 const AgentUtils::DEFAULT_WAIT_TIME = 30000;

void AgentUtils::assignParamsToQuestionNode(
    ScMemoryContext * ms_context,
    ScAddr const & actionNode,
    ScAddrVector const & params)
{
  SC_CHECK_PARAM(actionNode, "Invalid action node address passed to `assignParamsToQuestionNode`");

  for (size_t i = 0; i < params.size(); i++)
  {
    ScAddr edge = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, actionNode, params.at(i));
    ScAddr relation = IteratorUtils::getRoleRelation(ms_context, i + 1);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, relation, edge);
  }
}

ScAddr AgentUtils::createQuestionNode(ScMemoryContext * ms_context)
{
  ScAddr actionNode = ms_context->CreateNode(ScType::NodeConst);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::action, actionNode);
  return actionNode;
}

ScAddr AgentUtils::formActionNode(ScMemoryContext * ms_context, ScAddr const & actionClass, ScAddrVector const & params)
{
  SC_CHECK_PARAM(actionClass, "Invalid action class address");

  ScAddr actionNode = createQuestionNode(ms_context);
  assignParamsToQuestionNode(ms_context, actionNode, params);

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, actionClass, actionNode);

  return actionNode;
}

ScAddr AgentUtils::initAction(ScMemoryContext * ms_context, ScAddr const & actionClass, ScAddrVector const & params)
{
  SC_CHECK_PARAM(actionClass, "Invalid action class address passed to `initAction`");

  ScAddr actionNode = formActionNode(ms_context, actionClass, params);

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::action_initiated, actionNode);

  return actionNode;
}

bool AgentUtils::applyAction(
    ScMemoryContext * ms_context,
    ScAddr const & actionClass,
    ScAddrVector const & params,
    sc_uint32 const & waitTime)
{
  ScAddr actionNode = formActionNode(ms_context, actionClass, params);

  return applyAction(ms_context, actionNode, waitTime);
}

bool AgentUtils::applyAction(
    ScMemoryContext * ms_context,
    ScAddr const & actionNode,
    sc_uint32 const & waitTime,
    ScAddr onEventClassAddr)
{
  if (!onEventClassAddr.IsValid())
    onEventClassAddr = scAgentsCommon::CoreKeynodes::action_initiated;

  auto const & check = [](ScEventAddInputArc const & event)
  {
    return event.GetArcSourceElement() == scAgentsCommon::CoreKeynodes::action_finished ? SC_RESULT_OK : SC_RESULT_ERROR;
  };

  auto const & initialize = [ms_context, onEventClassAddr, actionNode]()
  {
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, onEventClassAddr, actionNode);
  };

  ScWaitCondition<ScEventAddInputArc> waiter(*ms_context, actionNode, check);
  waiter.SetOnWaitStartDelegate(initialize);
  return waiter.Wait(waitTime);
}

ScAddr AgentUtils::applyActionAndGetResultIfExists(
    ScMemoryContext * ms_context,
    ScAddr const & actionClass,
    ScAddrVector const & params,
    sc_uint32 const & waitTime)
{
  SC_CHECK_PARAM(actionClass, "Invalid action class address passed to `applyActionAndGetResultIfExists`");

  ScAddr actionNode = formActionNode(ms_context, actionClass, params);

  ScAddr result;
  if (applyAction(ms_context, actionNode, waitTime))
    result = IteratorUtils::getAnyByOutRelation(ms_context, actionNode, CoreKeynodes::nrel_answer);

  return result;
}

ScAddr AgentUtils::applyActionAndGetResultIfExists(
    ScMemoryContext * ms_context,
    ScAddr const & actionNode,
    sc_uint32 const & waitTime)
{
  SC_CHECK_PARAM(actionNode, "Invalid action node address passed to `applyActionAndGetResultIfExists`");

  ScAddr result;
  if (applyAction(ms_context, actionNode, waitTime))
    result = IteratorUtils::getAnyByOutRelation(ms_context, actionNode, CoreKeynodes::nrel_answer);

  return result;
}

void AgentUtils::finishAgentWork(
    ScMemoryContext * ms_context,
    ScAddr const & actionNode,
    ScAddrVector const & answerElements,
    bool isSuccess)
{
  SC_CHECK_PARAM(actionNode, "Invalid action node address passed to `finishAgentWork`");

  ScAddr answerNode = GenerationUtils::wrapInSet(ms_context, answerElements, ScType::NodeConstStruct);
  ScAddr edgeToAnswer = ms_context->CreateEdge(ScType::EdgeDCommonConst, actionNode, answerNode);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::nrel_answer, edgeToAnswer);

  finishAgentWork(ms_context, actionNode, isSuccess);
}

void AgentUtils::finishAgentWork(ScMemoryContext * ms_context, ScAddr const & actionNode, bool isSuccess)
{
  SC_CHECK_PARAM(actionNode, "Invalid action node address passed to `finishAgentWork`");

  ScAddr statusNode =
      isSuccess ? CoreKeynodes::action_finished_successfully : CoreKeynodes::action_finished_unsuccessfully;
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, statusNode, actionNode);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::action_finished, actionNode);
}

}  // namespace utils
