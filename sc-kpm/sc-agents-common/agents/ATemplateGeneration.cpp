/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include <iostream>
#include <thread>

#include "ATemplateGeneration.hpp"
#include "utils/IteratorUtils.hpp"
#include "utils/AgentUtils.hpp"
#include "utils/LogicRuleUtils.hpp"

using namespace std;
using namespace utils;

namespace scAgentsCommon
{

void ATemplateGeneration::fillGenParams(ScAddr & paramsSet, ScTemplateParams & genParams)
{
  ScIterator3Ptr iter_params = ms_context->Iterator3(paramsSet, ScType::EdgeAccessConstPosPerm, ScType::NodeConst);
  while (iter_params->Next())
  {
    ScAddr params = iter_params->Get(2);
    ScAddr templateNode = IteratorUtils::getFirstByOutRelation(ms_context.get(), params, CoreKeynodes::rrel_1);
    ScAddr argumentNode = IteratorUtils::getFirstByOutRelation(ms_context.get(), params, CoreKeynodes::rrel_2);
    if (argumentNode.IsValid() && templateNode.IsValid())
    {
      genParams.Add(ms_context->HelperGetSystemIdtf(templateNode), argumentNode);
      cout << "[param, arg]: " <<
           ms_context->HelperGetSystemIdtf(templateNode)
           << ", " << ms_context->HelperGetSystemIdtf(argumentNode) << endl;
    }
  }
}

SC_AGENT_IMPLEMENTATION(ATemplateGeneration)
{
  if (!edgeAddr.IsValid())
    return SC_RESULT_ERROR;

  ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);

  ScAddr paramSet = IteratorUtils::getFirstByOutRelation(ms_context.get(), questionNode, CoreKeynodes::rrel_1);
  if (!paramSet.IsValid())
  {
    cout << "No parameters set was found" << endl;
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  ScAddr statementNode = IteratorUtils::getFirstByOutRelation(ms_context.get(), questionNode, CoreKeynodes::rrel_2);
  if (!statementNode.IsValid())
  {
    cout << "No statement was found" << endl;
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  ScAddr elseStatement = LogicRuleUtils::getElseStatement(ms_context.get(), statementNode);

  ScTemplateParams genParams;
  fillGenParams(paramSet, genParams);

  ScTemplate scTemplate;
  ms_context->HelperBuildTemplate(scTemplate, elseStatement);

  ScTemplateGenResult result;
  bool isGenerated = ms_context->HelperGenTemplate(scTemplate, result, genParams);
  cout << "Generation Result: " << isGenerated << "\n";

  ScAddr answer = ms_context->CreateNode(ScType::NodeConst);
  AgentUtils::finishAgentWork(ms_context.get(), questionNode, answer, isGenerated);
  return SC_RESULT_OK;
}

} // namespace scAgentsCommon
