/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "constants/SubjDomainAliases.hpp"
#include "keynodes/Keynodes.hpp"

#include "ScSearchGetDecompositionAgent.hpp"

using namespace utils;

namespace scSearch
{
SC_AGENT_IMPLEMENTATION(ScSearchGetDecompositionAgent)
{
  ScAddr questionNode = m_memoryCtx.GetEdgeTarget(edgeAddr);
  if (!checkActionClass(questionNode))
    return SC_RESULT_OK;

  SC_LOG_DEBUG("GetDecompositionAgent started");

  int level = 1;
  ScAddr subjDomainAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, questionNode, scAgentsCommon::CoreKeynodes::rrel_1);
  ScAddr levelAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, questionNode, scAgentsCommon::CoreKeynodes::rrel_2);
  ScAddr langAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, questionNode, scAgentsCommon::CoreKeynodes::rrel_3);

  if (!m_memoryCtx.IsElement(subjDomainAddr))
  {
    SC_LOG_ERROR("GetDecompositionAgent: subject domain node not found.");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }
  if (m_memoryCtx.IsElement(levelAddr))
    level = stoi(CommonUtils::getLinkContent(&m_memoryCtx, levelAddr));
  if (!m_memoryCtx.IsElement(langAddr))
  {
    SC_LOG_ERROR("GetDecompositionAgent: language node not found.");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  ScAddr answerLink = m_memoryCtx.CreateLink();

  ScAddrVector decomposition = getDecomposition(subjDomainAddr);
  json answerJSON {
        {CommonUtils::getScWebAddr(subjDomainAddr), {
              {"idtf", CommonUtils::getMainIdtf(&m_memoryCtx, subjDomainAddr, {langAddr})},
              {"decomposition", getJSONDecomposition(decomposition, level, langAddr)},
              {"position", 0}
        }}
  };

  std::string answerJSONContent = answerJSON.dump();
  SC_LOG_DEBUG(answerJSONContent);
  m_memoryCtx.SetLinkContent(answerLink, answerJSONContent);

  ScAddrVector answerElements = {answerLink};

  utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, answerElements, true);
  SC_LOG_DEBUG("GetDecompositionAgent finished");
  return SC_RESULT_OK;
}

bool ScSearchGetDecompositionAgent::checkActionClass(ScAddr const & actionNode)
{
  return m_memoryCtx.HelperCheckEdge(Keynodes::action_get_decomposition, actionNode, ScType::EdgeAccessConstPosPerm);
}

ScAddrVector ScSearchGetDecompositionAgent::getDecomposition(ScAddr const & subjDomainAddr)
{
  SC_LOG_DEBUG("Main section: " + ms_context->HelperGetSystemIdtf(subjDomainAddr));
  ScAddrVector decomposition;

  ScTemplate decompositionTemplate;
  decompositionTemplate.Quintuple(
        ScType::NodeVar >> SubjDomainAliases::DECOMPOSITION_TUPLE,
        ScType::EdgeDCommonVar,
        subjDomainAddr,
        ScType::EdgeAccessVarPosPerm,
        Keynodes::nrel_entity_decomposition
  );
  decompositionTemplate.Quintuple(
        SubjDomainAliases::DECOMPOSITION_TUPLE,
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVar >> SubjDomainAliases::SECTION_NODE,
        ScType::EdgeAccessVarPosPerm,
        scAgentsCommon::CoreKeynodes::rrel_1
  );
  ScTemplateSearchResult result;
  ms_context->HelperSearchTemplate(decompositionTemplate, result);
  if (!result.IsEmpty())
  {
    ScAddr tupleNode = result[0][SubjDomainAliases::DECOMPOSITION_TUPLE];
    ScAddr subSection = result[0][SubjDomainAliases::SECTION_NODE];
    SC_LOG_DEBUG("GetDecompositionAgent: section is " + ms_context->HelperGetSystemIdtf(subjDomainAddr) + ".");
    SC_LOG_DEBUG("GetDecompositionAgent: subsection is " + ms_context->HelperGetSystemIdtf(subSection) + ".");
    decomposition.push_back(subSection);

    ScAddr nextSubSection = utils::IteratorUtils::getNextFromSet(ms_context.get(), tupleNode, subSection);
    while (ms_context->IsElement(nextSubSection))
    {
      decomposition.push_back(nextSubSection);
      subSection = nextSubSection;
      SC_LOG_DEBUG("GetDecompositionAgent: section is " + ms_context->HelperGetSystemIdtf(subjDomainAddr) + ".");
      SC_LOG_DEBUG("GetDecompositionAgent: sub section is " + m_memoryCtx.HelperGetSystemIdtf(subSection) + ".");
      nextSubSection = utils::IteratorUtils::getNextFromSet(ms_context.get(), tupleNode, subSection);
    }
  }
  return decomposition;
}

json ScSearchGetDecompositionAgent::getJSONDecomposition(ScAddrVector const & decomposition, int level, ScAddr const & langAddr)
{
  json decompositionJSON;
  for (size_t index = 0; index < decomposition.size(); index++)
  {
    json item;
    item["position"] = index;
    item["idtf"] = CommonUtils::getMainIdtf(&m_memoryCtx, decomposition[index], {langAddr});
    if (level != 1)
      item["decomposition"] = getJSONDecomposition(getDecomposition(decomposition[index]), level - 1, langAddr);
    else
      item["decomposition"] = {};
    decompositionJSON[CommonUtils::getScWebAddr(decomposition[index])] = item;
  }

  return decompositionJSON;
}
}
