/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "EraseElementsAgent.hpp"

namespace scAgentsCommon
{
SC_AGENT_IMPLEMENTATION(EraseElementsAgent)
{
  ScAddr questionNode = m_memoryCtx.GetEdgeTarget(edgeAddr);
  if (!checkActionClass(questionNode))
    return SC_RESULT_OK;

  SC_LOG_DEBUG("EraseElementsAgent started");

  ScAddr erasableElementsSet =
      utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, CoreKeynodes::rrel_1);

  ScAddrVector elementsToErase;
  ScAddrVector answerElements;
  ScIterator3Ptr iterator3 =
      ms_context->Iterator3(erasableElementsSet, ScType::EdgeAccessConstPosPerm, ScType::Unknown);
  while (iterator3->Next())
  {
    ScAddr element = iterator3->Get(2);

    if (!m_memoryCtx.HelperCheckEdge(
            EraseElementsKeynodes::unerasable_elements, element, ScType::EdgeAccessConstPosPerm))
      elementsToErase.push_back(element);
    else
      answerElements.push_back(element);
  }
  for (ScAddr element : elementsToErase)
    m_memoryCtx.EraseElement(element);

  ScAddr answerSet = m_memoryCtx.CreateNode(ScType::NodeConst);
  size_t answerSize = answerElements.size();
  for (size_t i = 0; i < answerSize; i++)
  {
    ScAddr edge = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerSet, answerElements[i]);
    answerElements.push_back(edge);
  }
  answerElements.push_back(answerSet);

  utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, answerElements, true);
  SC_LOG_DEBUG("EraseElementsAgent finished");
  return SC_RESULT_OK;
}

bool EraseElementsAgent::checkActionClass(ScAddr const & actionNode)
{
  return m_memoryCtx.HelperCheckEdge(
      EraseElementsKeynodes::action_erase_elements, actionNode, ScType::EdgeAccessConstPosPerm);
}

}  // namespace scAgentsCommon
