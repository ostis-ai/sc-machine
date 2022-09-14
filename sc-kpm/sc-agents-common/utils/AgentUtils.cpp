/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "AgentUtils.hpp"

#include <sc-memory/sc_wait.hpp>

#include "IteratorUtils.hpp"
#include "GenerationUtils.hpp"
#include "keynodes/coreKeynodes.hpp"

using namespace std;
using namespace scAgentsCommon;

namespace utils
{
sc_uint32 const AgentUtils::DEFAULT_WAIT_TIME = 30000;

void AgentUtils::assignParamsToQuestionNode(
    ScMemoryContext * ms_context,
    const ScAddr & questionNode,
    const ScAddrVector & params)
{
  SC_CHECK_PARAM(questionNode, ("Invalid question node address"));

  for (size_t i = 0; i < params.size(); i++)
  {
    ScAddr edge = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, questionNode, params.at(i));
    ScAddr relation = IteratorUtils::getRoleRelation(ms_context, i + 1);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, relation, edge);
  }
}

ScAddr AgentUtils::createQuestionNode(ScMemoryContext * ms_context)
{
  ScAddr questionNode = ms_context->CreateNode(ScType::NodeConst);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::question, questionNode);
  return questionNode;
}

bool AgentUtils::waitAgentResult(ScMemoryContext * ms_context, const ScAddr & questionNode, const sc_uint32 waitTime)
{
  SC_CHECK_PARAM(questionNode, ("Invalid question node address"));

  auto check = [ms_context](ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) {
    return ms_context->HelperCheckEdge(CoreKeynodes::nrel_answer, edgeAddr, ScType::EdgeAccessConstPosPerm);
  };
  ScWaitCondition<ScEventAddOutputEdge> waiter(*ms_context, questionNode, SC_WAIT_CHECK(check));
  return waiter.Wait(waitTime);
}

ScAddr AgentUtils::formActionNode(ScMemoryContext * ms_context, const ScAddr & actionClass, const ScAddrVector & params)
{
  SC_CHECK_PARAM(actionClass, ("Invalid action class address"));

  ScAddr actionNode = createQuestionNode(ms_context);
  assignParamsToQuestionNode(ms_context, actionNode, params);

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, actionClass, actionNode);

  return actionNode;
}

ScAddr AgentUtils::initAgent(ScMemoryContext * ms_context, const ScAddr & questionClass, const ScAddrVector & params)
{
  return initAction(ms_context, questionClass, params);
}

ScAddr AgentUtils::initAction(ScMemoryContext * ms_context, const ScAddr & questionClass, const ScAddrVector & params)
{
  SC_CHECK_PARAM(questionClass, ("Invalid question class address"));

  ScAddr questionNode = formActionNode(ms_context, questionClass, params);

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::question_initiated, questionNode);

  return questionNode;
}

bool AgentUtils::applyAction(
    ScMemoryContext * ms_context,
    const ScAddr & actionClass,
    const ScAddrVector & params,
    const sc_uint32 & waitTime)
{
  ScAddr actionNode = formActionNode(ms_context, actionClass, params);

  return applyAction(ms_context, actionNode, waitTime);
}

bool AgentUtils::applyAction(ScMemoryContext * ms_context, const ScAddr & actionNode, const sc_uint32 & waitTime)
{
  auto check = [](ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) {
    return otherAddr == scAgentsCommon::CoreKeynodes::question_finished;
  };

  auto initialize = [ms_context, actionNode]() {
    ms_context->CreateEdge(
        ScType::EdgeAccessConstPosPerm, scAgentsCommon::CoreKeynodes::question_initiated, actionNode);
  };

  ScWaitCondition<ScEventAddInputEdge> waiter(*ms_context, actionNode, SC_WAIT_CHECK(check));
  return waiter.Wait(waitTime, initialize);
}

ScAddr AgentUtils::initAgentAndWaitResult(
    ScMemoryContext * ms_context,
    const ScAddr & actionClass,
    const ScAddrVector & params,
    const sc_uint32 & waitTime)
{
  return getActionResultIfExists(ms_context, actionClass, params, waitTime);
}

ScAddr AgentUtils::getActionResultIfExists(
    ScMemoryContext * ms_context,
    const ScAddr & actionClass,
    const ScAddrVector & params,
    const sc_uint32 & waitTime)
{
  SC_CHECK_PARAM(actionClass, ("Invalid action class address"));

  ScAddr actionNode = formActionNode(ms_context, actionClass, params);

  ScAddr result;
  if (applyAction(ms_context, actionNode))
    result = IteratorUtils::getAnyByOutRelation(ms_context, actionNode, CoreKeynodes::nrel_answer);

  return result;
}

ScAddr AgentUtils::getActionResultIfExists(
    ScMemoryContext * ms_context,
    const ScAddr & actionNode,
    const sc_uint32 & waitTime)
{
  SC_CHECK_PARAM(actionNode, ("Invalid action node address"));

  ScAddr result;
  if (applyAction(ms_context, actionNode))
    result = IteratorUtils::getAnyByOutRelation(ms_context, actionNode, CoreKeynodes::nrel_answer);

  return result;
}

void AgentUtils::finishAgentWork(
    ScMemoryContext * ms_context,
    const ScAddr & questionNode,
    const ScAddr & answerNode,
    bool isSuccess)
{
  SC_CHECK_PARAM(questionNode, ("Invalid question node address"));
  SC_CHECK_PARAM(answerNode, ("Invalid answer node address"));

  ScAddr edgeToAnswer = ms_context->CreateEdge(ScType::EdgeDCommonConst, questionNode, answerNode);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::nrel_answer, edgeToAnswer);

  finishAgentWork(ms_context, questionNode, isSuccess);
}

void AgentUtils::finishAgentWork(
    ScMemoryContext * ms_context,
    const ScAddr & questionNode,
    const ScAddrVector & answerElements,
    bool isSuccess)
{
  SC_CHECK_PARAM(questionNode, ("Invalid question node address"));

  ScAddr answerNode = GenerationUtils::wrapInSet(ms_context, answerElements, ScType::NodeConstStruct);
  ScAddr edgeToAnswer = ms_context->CreateEdge(ScType::EdgeDCommonConst, questionNode, answerNode);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::nrel_answer, edgeToAnswer);

  finishAgentWork(ms_context, questionNode, isSuccess);
}

void AgentUtils::finishAgentWork(ScMemoryContext * ms_context, const ScAddr & questionNode, bool isSuccess)
{
  SC_CHECK_PARAM(questionNode, ("Invalid question node address"));

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::question_finished, questionNode);
  ScAddr statusNode =
      isSuccess ? CoreKeynodes::question_finished_successfully : CoreKeynodes::question_finished_unsuccessfully;
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, statusNode, questionNode);
}

}  // namespace utils
