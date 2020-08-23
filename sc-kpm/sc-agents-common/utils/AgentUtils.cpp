/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <string>

#include <sc-memory/sc_wait.hpp>

#include "AgentUtils.hpp"
#include "IteratorUtils.hpp"
#include "keynodes/coreKeynodes.hpp"

using namespace std;
using namespace scAgentsCommon;

namespace utils
{

void AgentUtils::assignParamsToQuestionNode(
      ScMemoryContext * ms_context,
      const ScAddr & questionNode,
      const vector<ScAddr> & params)
{
  ScAddr numberRelation, arc;
  for (vector<int>::size_type i = 0; i < params.size(); i++)
  {
    // TODO: Replace by using array of rrel_ keynodes
    numberRelation = ms_context->HelperResolveSystemIdtf("rrel_" + to_string(i + 1), ScType::NodeConstRole);
    if (numberRelation.IsValid())
    {
      arc = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, questionNode, params[i]);
      ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, numberRelation, arc);
    }
    else break;
  }
}

ScAddr AgentUtils::createQuestionNode(ScMemoryContext * ms_context)
{
  ScAddr questionNode = ms_context->CreateNode(ScType::NodeConst);
  ScAddr question = CoreKeynodes::question;
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, question, questionNode);
  return questionNode;
}

bool AgentUtils::waitAgentResult(ScMemoryContext * ms_context, const ScAddr & questionNode)
{
  auto check = [ms_context](ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
  {
    return ms_context->HelperCheckEdge(CoreKeynodes::nrel_answer, edgeAddr, ScType::EdgeAccessConstPosPerm);
  };
  ScWaitCondition<ScEventAddOutputEdge> waiter(*ms_context, questionNode, SC_WAIT_CHECK(check));
  return waiter.Wait(30000);
}

ScAddr AgentUtils::initAgent(ScMemoryContext * ms_context, const ScAddr & questionName, const vector<ScAddr> & params)
{
  ScAddr questionNode = createQuestionNode(ms_context);
  assignParamsToQuestionNode(ms_context, questionNode, params);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::question, questionNode);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::question_initiated, questionNode);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, questionName, questionNode);
  return questionNode;
}

ScAddr AgentUtils::initAgentAndWaitResult(
      ScMemoryContext * ms_context,
      const ScAddr & questionName,
      const vector<ScAddr> & params)
{
  ScAddr questionNode = initAgent(ms_context, questionName, params);
  ScAddr answer;
  if (waitAgentResult(ms_context, questionNode))
  {
    ScAddr nrelAnswer = CoreKeynodes::nrel_answer;
    answer = IteratorUtils::getFirstByOutRelation(ms_context, questionNode, nrelAnswer);
  }
  return answer;
}

void AgentUtils::finishAgentWork(
      ScMemoryContext * ms_context,
      const ScAddr & questionNode,
      const ScAddr & answer,
      bool isSuccess)
{
  if (answer.IsValid())
  {
    ScAddr edgeToAnswer = ms_context->CreateEdge(ScType::EdgeDCommonConst, questionNode, answer);
    ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::nrel_answer, edgeToAnswer);
  }
  finishAgentWork(ms_context, questionNode, isSuccess);
}

void AgentUtils::finishAgentWork(ScMemoryContext * ms_context, const ScAddr & questionNode, bool isSuccess)
{
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, CoreKeynodes::question_finished, questionNode);
  ScAddr status = isSuccess
                  ? CoreKeynodes::question_finished_successfully
                  : CoreKeynodes::question_finished_unsuccessfully;
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, status, questionNode);
}
}
