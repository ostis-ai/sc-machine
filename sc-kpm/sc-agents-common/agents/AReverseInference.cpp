/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/
#include <map>

#include "AReverseInference.hpp"
#include "utils/IteratorUtils.hpp"
#include "utils/AgentUtils.hpp"
#include "utils/LogicRuleUtils.hpp"

using namespace std;
using namespace utils;

namespace scAgentsCommon
{
bool AReverseInference::generateStatement(ScAddr & statement, const ScTemplateParams & templateParams)
{
  ScTemplate statementTemplate;
  ms_context->HelperBuildTemplate(statementTemplate, statement);
  ScTemplateGenResult result;
  return ms_context->HelperGenTemplate(statementTemplate, result, templateParams);
}

void AReverseInference::createTemplateParams(ScAddr & statement, vector<ScAddr> argumentList,
                                             ScTemplateParams & templateParam)
{
  ScIterator3Ptr varIterator = ms_context->Iterator3(
        statement,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeVar);
  while (varIterator->Next())
  {
    ScAddr var = varIterator->Get(2);
    ScAddr argumentOfVar;
    ScIterator5Ptr classesIterator = ms_context->Iterator5(
          ScType::NodeConstClass,
          ScType::EdgeAccessVarPosPerm,
          var,
          ScType::EdgeAccessConstPosPerm,
          statement);
    while (classesIterator->Next())
    {
      ScAddr varClass = classesIterator->Get(0);
      for (auto & argument : argumentList)
      {
        if (ms_context->HelperCheckEdge(varClass, argument, ScType::EdgeAccessConstPosPerm))
        {
          argumentOfVar = argument;
          break;
        }
      }
      if (argumentOfVar.IsValid())
      {
        string varName = ms_context->HelperGetSystemIdtf(var);
        templateParam.Add(varName, argumentOfVar);
        break;
      }
    }
  }
}

vector<ScAddr> AReverseInference::findRulesWithKeyElement(ScAddr & keyElement)
{
  return IteratorUtils::getAllByInRelation(ms_context.get(), keyElement, CoreKeynodes::rrel_solver_key_sc_element);
}

vector<ScAddr> AReverseInference::getRequestKeyElementList(ScAddr & request)
{
  vector<ScAddr> classes = IteratorUtils::getAllWithType(ms_context.get(), request, ScType::NodeConstClass);
  vector<ScAddr> roleRelations = IteratorUtils::getAllWithType(ms_context.get(), request, ScType::NodeConstRole);
  vector<ScAddr> noRoleRelations = IteratorUtils::getAllWithType(ms_context.get(), request, ScType::NodeConstNoRole);
  vector<ScAddr> keyElementList;
  move(classes.begin(), classes.end(), back_inserter(keyElementList));
  move(roleRelations.begin(), roleRelations.end(), back_inserter(keyElementList));
  move(noRoleRelations.begin(), noRoleRelations.end(), back_inserter(keyElementList));
  return keyElementList;
}

SC_AGENT_IMPLEMENTATION(AReverseInference)
{
  if (!edgeAddr.IsValid())
    return SC_RESULT_ERROR;

  ScAddr questionNode = ms_context->GetEdgeTarget(edgeAddr);
  ScAddr request = IteratorUtils::getFirstFromSet(ms_context.get(), questionNode);
  if (!request.IsValid())
  {
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  vector<ScAddr> argumentList = IteratorUtils::getAllWithType(ms_context.get(), request, ScType::NodeConst);
  vector<ScAddr> keyElementList = getRequestKeyElementList(request);

  ScAddr usedRule;
  ScTemplate requestTemplate;
  ms_context->HelperBuildTemplate(requestTemplate, request);
  for (auto & keyElement : keyElementList)
  {
    vector<ScAddr> ruleList = findRulesWithKeyElement(keyElement);
    for (auto & rule : ruleList)
    {
      ScAddr ifStatement = LogicRuleUtils::getIfStatement(ms_context.get(), rule);
      ScAddr elseStatement = LogicRuleUtils::getElseStatement(ms_context.get(), rule);

      ScTemplateParams templateParams;
      createTemplateParams(ifStatement, argumentList, templateParams);
      ScTemplate ifStatementTemplate;
      if (ms_context->HelperBuildTemplate(ifStatementTemplate, ifStatement, templateParams))
      {
        ScTemplateSearchResult searchResult;
        if (ms_context->HelperSearchTemplate(ifStatementTemplate, searchResult))
        {
          bool isGenerated = generateStatement(elseStatement, templateParams);
          if (isGenerated && ms_context->HelperSearchTemplate(requestTemplate, searchResult))
          {
            usedRule = rule;
            break;
          }
        }
        else
        {
          //add analyzing of ifStatement and try to find rules for use it
        }
      }
    }
    if (usedRule.IsValid())
      break;
  }
  ScAddr decisionTree = ms_context->CreateNode(ScType::NodeConst);
  ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, decisionTree, usedRule);

  AgentUtils::finishAgentWork((ScMemoryContext *) ms_context.get(), questionNode, decisionTree);
  return SC_RESULT_OK;
}
}
