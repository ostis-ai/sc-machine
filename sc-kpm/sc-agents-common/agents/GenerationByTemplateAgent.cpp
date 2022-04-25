/*
 * Copyright (c) 2022 Intelligent Semantic Systems LLC, All rights reserved.
 */

#include "sc-memory/scs/scs_types.hpp"

#include "utils/CommonUtils.hpp"
#include "utils/IteratorUtils.hpp"

#include "GenerationByTemplateAgent.hpp"

namespace scAgentsCommon
{
SC_AGENT_IMPLEMENTATION(GenerationByTemplateAgent)
{
  ScAddr questionNode = otherAddr;
  if (!checkActionClass(questionNode))
    return SC_RESULT_OK;

  SC_LOG_DEBUG("GenerationByTemplateAgent started")

  ScAddr templateLinkAddr = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, CoreKeynodes::rrel_1);
  ScAddr parameterNodeAddr =
      utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, CoreKeynodes::rrel_2);

  if (!templateLinkAddr.IsValid())
  {
    SC_LOG_ERROR("GenerationByTemplateAgent: template link not found.")
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR;
  }
  if (!parameterNodeAddr.IsValid())
  {
    SC_LOG_ERROR("GenerationByTemplateAgent: parameter node not found.")
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR;
  }

  std::string templateIdtf = utils::CommonUtils::getLinkContent(&m_memoryCtx, templateLinkAddr);
  if (templateIdtf.empty())
  {
    SC_LOG_ERROR("GenerationByTemplateAgent: template name string is empty.")
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR;
  }

  ScAddr ruleStatement = m_memoryCtx.HelperFindBySystemIdtf(templateIdtf);
  if (!ruleStatement.IsValid())
  {
    SC_LOG_ERROR("GenerationByTemplateAgent: rule statement not found.")
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR;
  }

  ScTemplateParams genParams;
  fillGenParams(parameterNodeAddr, genParams);

  bool isGenerated = generateByTemplate(genParams, ruleStatement);
  if (!isGenerated)
  {
    SC_LOG_ERROR("GenerationByTemplateAgent: unable to generate by template.")
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR;
  }

  utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
  SC_LOG_DEBUG("GenerationByTemplateAgent finished")
  return SC_RESULT_OK;
}

bool scAgentsCommon::GenerationByTemplateAgent::checkActionClass(ScAddr const & actionAddr)
{
  return m_memoryCtx.HelperCheckEdge(
      CoreKeynodes::action_generate_by_template_with_param_resolving, actionAddr, ScType::EdgeAccessConstPosPerm);
}

void scAgentsCommon::GenerationByTemplateAgent::fillGenParams(ScAddr & parameterNodeAddr, ScTemplateParams & genParams)
{
  ScIterator3Ptr parameterIterator =
      m_memoryCtx.Iterator3(parameterNodeAddr, ScType::EdgeAccessConstPosPerm, ScType::EdgeDCommonConst);
  while (parameterIterator->Next())
  {
    ScAddr commonParamEdge = parameterIterator->Get(2);
    ScAddr paramNameLink = m_memoryCtx.GetEdgeSource(commonParamEdge);
    ScAddr auxiliaryNode = m_memoryCtx.GetEdgeTarget(commonParamEdge);
    ScIterator3Ptr argumentIterator =
        m_memoryCtx.Iterator3(auxiliaryNode, ScType::EdgeAccessConstPosPerm, ScType::EdgeDCommonConst);
    if (argumentIterator->Next())
    {
      ScAddr commonArgumentEdge = argumentIterator->Get(2);
      ScAddr valueArgumentNode = m_memoryCtx.GetEdgeSource(commonArgumentEdge);
      ScAddr typeArgumentNode = m_memoryCtx.GetEdgeTarget(commonArgumentEdge);

      std::string linkParameterContent = utils::CommonUtils::getLinkContent(&m_memoryCtx, paramNameLink);
      std::string typeArgumentNodeContent = utils::CommonUtils::getLinkContent(&m_memoryCtx, typeArgumentNode);

      ScType type = scs::TypeResolver::GetKeynodeType(typeArgumentNodeContent);

      if (type.IsLink())
        genParams.Add(linkParameterContent, valueArgumentNode);
      else if (type.IsNode())
      {
        std::string argumentTypeContent = utils::CommonUtils::getLinkContent(&m_memoryCtx, valueArgumentNode);
        ScAddr valueParameterNode = m_memoryCtx.HelperResolveSystemIdtf(argumentTypeContent, type);
        genParams.Add(linkParameterContent, valueParameterNode);
      }
    }
  }
}

bool scAgentsCommon::GenerationByTemplateAgent::generateByTemplate(ScTemplateParams & genParams, ScAddr & ruleStatement)
{
  bool isGenerated;

  ScTemplate scTemplate;
  ms_context->HelperBuildTemplate(scTemplate, ruleStatement);
  ScTemplateGenResult genResult;
  isGenerated = m_memoryCtx.HelperGenTemplate(scTemplate, genResult, genParams);

  return isGenerated;
}

}  // namespace scAgentsCommon
