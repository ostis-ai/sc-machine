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

void AgentUtils::assignParamsToQuestionNode(
      ScMemoryContext * ms_context,
      const ScAddr & questionNode,
      const ScAddrVector & params)
{
  SC_CHECK_PARAM(questionNode, ("Invalid question node address"))

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
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm,  CoreKeynodes::question, questionNode);
  return questionNode;
}

bool AgentUtils::waitAgentResult(ScMemoryContext * ms_context, const ScAddr & questionNode, const uint32_t waitTime)
{
  SC_CHECK_PARAM(questionNode, ("Invalid question node address"))

  auto check = [ms_context](ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
  {
    return ms_context->HelperCheckEdge(CoreKeynodes::nrel_answer, edgeAddr, ScType::EdgeAccessConstPosPerm);
  };
  ScWaitCondition<ScEventAddOutputEdge> waiter(*ms_context, questionNode, SC_WAIT_CHECK(check));
  return waiter.Wait(waitTime);
}

ScAddr AgentUtils::initAgent(ScMemoryContext * ms_context, const ScAddr & questionClass, const ScAddrVector & params)
{
  SC_CHECK_PARAM(questionClass, ("Invalid question class address"))

  ScAddr questionNode = createQuestionNode(ms_context);
  assignParamsToQuestionNode(ms_context, questionNode, params);

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, questionClass, questionNode);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::question_initiated, questionNode);
  return questionNode;
}

ScAddr AgentUtils::initAgentAndWaitResult(
      ScMemoryContext * ms_context,
      const ScAddr & questionClass,
      const ScAddrVector & params)
{
  SC_CHECK_PARAM(questionClass, ("Invalid question class address"))

  ScAddr questionNode = initAgent(ms_context, questionClass, params);
  if (waitAgentResult(ms_context, questionNode))
  {
    ScAddr nrelAnswer = CoreKeynodes::nrel_answer;
    return IteratorUtils::getAnyByOutRelation(ms_context, questionNode, nrelAnswer);
  }
  return {};
}

void AgentUtils::finishAgentWork(
      ScMemoryContext * ms_context,
      const ScAddr & questionNode,
      const ScAddr & answerNode,
      bool isSuccess)
{
  SC_CHECK_PARAM(questionNode, ("Invalid question node address"))
  SC_CHECK_PARAM(answerNode, ("Invalid answer node address"))

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
  SC_CHECK_PARAM(questionNode, ("Invalid question node address"))

  ScAddr answerNode = GenerationUtils::wrapInSet(ms_context, answerElements, ScType::NodeConstStruct);
  ScAddr edgeToAnswer = ms_context->CreateEdge(ScType::EdgeDCommonConst, questionNode, answerNode);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::nrel_answer, edgeToAnswer);

  finishAgentWork(ms_context, questionNode, isSuccess);
}

void AgentUtils::finishAgentWork(ScMemoryContext * ms_context, const ScAddr & questionNode, bool isSuccess)
{
  SC_CHECK_PARAM(questionNode, ("Invalid question node address"))

  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::question_finished, questionNode);
  ScAddr statusNode = isSuccess
                  ? CoreKeynodes::question_finished_successfully
                  : CoreKeynodes::question_finished_unsuccessfully;
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, statusNode, questionNode);
}

}
