/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_wait.hpp"
#include "nlDialogue.hpp"

namespace nl
{

ScAddr ADialogueProcessMessageAgent::ms_kNrelOutputText;
ScAddr ADialogueProcessMessageAgent::ms_kCommandGenCmdResultText;

SC_AGENT_IMPLEMENTATION(ADialogueProcessMessageAgent)
{
  ScAddr const dialogueAddr = listenAddr;
  ScAddr const itemAddr = otherAddr;
  SC_ASSERT(dialogueAddr == Keynodes::msMainNLDialogueInstance, ());

  if (!m_memoryCtx.GetElementType(itemAddr).IsLink())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  if (itemAddr.IsValid())
  {
    // just if author is not a system
    ScTemplate authorTempl;
    authorTempl.TripleWithRelation(
          itemAddr,
          ScType::EdgeDCommonVar,
          Keynodes::msSelf,
          ScType::EdgeAccessVarPosPerm,
          Keynodes::msNrelAuthor);

    ScTemplateSearchResult authSearchRes;
    if (m_memoryCtx.HelperSearchTemplate(authorTempl, authSearchRes))
      return SC_RESULT_ERROR_INVALID_STATE;

    // run api ai
    ScAddr const cmdAddr = ScAgentAction::CreateCommand(m_memoryCtx, Keynodes::msCmdProcessUserTextMessage, { itemAddr });
    if (!ScAgentAction::InitiateCommandWait(m_memoryCtx, cmdAddr))
      return SC_RESULT_ERROR_INVALID_STATE;
    
    ScAddr const resultAddr = ScAgentAction::GetCommandResultAddr(m_memoryCtx, cmdAddr);
    SC_ASSERT(resultAddr.IsValid(), ());

    // for initiated commands we need to wait a result and translate it into text
    {
      ScIterator5Ptr it = m_memoryCtx.Iterator5(
        resultAddr,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst,
        ScType::EdgeAccessConstPosPerm,
        ScKeynodes::GetRrelIndex(0));
      
      if (it->Next())
      {
        ScAddr const cmd = it->Get(2);
        /// TODO: check if specified addr is a command
        if (ScAgentAction::InitiateCommandWait(m_memoryCtx, cmd))
        {
          ScAddr const genTextCmd = ScAgentAction::CreateCommand(m_memoryCtx, ms_kCommandGenCmdResultText, { cmd });
          if (ScAgentAction::InitiateCommandWait(m_memoryCtx, genTextCmd))
          {
            ScAddr const genTextResult = ScAgentAction::GetCommandResultAddr(m_memoryCtx, genTextCmd);
            SC_ASSERT(genTextResult.IsValid(), ());

            // get text and link as output text
            ScIterator3Ptr it = m_memoryCtx.Iterator3(
              genTextResult,
              ScType::EdgeAccessConstPosPerm,
              ScType::Link);

            if (it->Next())
            {
              ScTemplate templGen;
              templGen.TripleWithRelation(
                resultAddr,
                ScType::EdgeDCommonVar,
                it->Get(2),
                ScType::EdgeAccessVarPosPerm,
                ms_kNrelOutputText);
              
              ScTemplateGenResult res;
              if (!m_memoryCtx.HelperGenTemplate(templGen, res))
                return SC_RESULT_ERROR;
            }
          }
        }
      }
    }


    /// TODO: add blockers, to prevent race conditions with last element
    // append it into dialogue
    ScTemplate lastItemTemplate;
    lastItemTemplate.TripleWithRelation(
          Keynodes::msMainNLDialogueInstance,
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVar >> "_last_item",
          ScType::EdgeAccessVarPosPerm >> "_last_item_attr",
          Keynodes::msRrelLastItem);

    lastItemTemplate.TripleWithRelation(
          "_last_item",
          ScType::EdgeDCommonVar,
          Keynodes::msSelf,
          ScType::EdgeAccessVarPosPerm,
          Keynodes::msNrelAuthor);

    ScAddr lastItemAddr;
    ScTemplateSearchResult searchResult;
    if (m_memoryCtx.HelperSearchTemplate(lastItemTemplate, searchResult))
    {
      ScTemplateSearchResultItem item = searchResult[0];
      lastItemAddr = item["_last_item"];
      // remove attr edge
      m_memoryCtx.EraseElement(item["_last_item_attr"]);
    }

    // if previous item exist, then append item order relation
    if (lastItemAddr.IsValid())
    {
      ScTemplate nextItemTemplate;
      nextItemTemplate.TripleWithRelation(
            lastItemAddr,
            ScType::EdgeDCommonVar,
            resultAddr,
            ScType::EdgeAccessVarPosPerm,
            Keynodes::msNrelItemOrder);

      ScTemplateGenResult genResult;
      if (!m_memoryCtx.HelperGenTemplate(nextItemTemplate, genResult))
        return SC_RESULT_ERROR_INVALID_STATE;
    }

    // append last item attr
    {
      ScTemplateGenParams params;
      params.add("_last_item", resultAddr);

      ScTemplateGenResult genResult;
      if (!m_memoryCtx.HelperGenTemplate(lastItemTemplate, genResult, params))
        return SC_RESULT_ERROR_INVALID_STATE;
    }

    return SC_RESULT_OK;
  }

  return SC_RESULT_ERROR_INVALID_STATE;
}

} // namespace nl
