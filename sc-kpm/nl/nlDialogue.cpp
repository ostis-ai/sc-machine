/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc-memory/cpp/sc_wait.hpp"
#include "nlDialogue.hpp"

namespace nl
{

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

    /*
     * command_process_user_text_message
     * _<- _command_instance
     * (* _-> _message;; *);;
     */
    ScTemplate cmdProcessTextTemplate;
    cmdProcessTextTemplate.Triple(
          Keynodes::msCmdProcessUserTextMessage,
          ScType::EdgeAccessVarPosPerm,
          ScType::NodeVar >> "_command_instance");
    cmdProcessTextTemplate.Triple(
          "_command_instance",
          ScType::EdgeAccessVarPosPerm,
          itemAddr);

    ScTemplateGenResult result;
    if (m_memoryCtx.HelperGenTemplate(cmdProcessTextTemplate, result))
    {
      const ScAddr cmdAddr = result["_command_instance"];
      SC_ASSERT(cmdAddr.IsValid(), ());

      // initiate command
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::msCommandInitiated, cmdAddr);

      // wait until command finish
      ScWaitCondition<ScEventAddInputEdge> waiter(
            m_memoryCtx, cmdAddr, [this](ScAddr const & addr,
            ScAddr const & edgeAddr,
            ScAddr const & otherAddr)
      {
        return (otherAddr == Keynodes::msCommandFinished);
      });

      if (!waiter.Wait())
        return SC_RESULT_ERROR_INVALID_STATE;

      // parse result and append it into dialogue
      ScTemplate resultTemplate;
      resultTemplate.TripleWithRelation(
            cmdAddr,
            ScType::EdgeDCommonVar,
            ScType::NodeVarStruct >> "_result",
            ScType::EdgeAccessVarPosPerm,
            Keynodes::msNrelResult);

      ScTemplateSearchResult searchResult;
      if (!m_memoryCtx.HelperSearchTemplate(resultTemplate, searchResult))
        return SC_RESULT_ERROR_INVALID_STATE;

      ScAddr resultAddr;
      {
        ScTemplateSearchResultItem item = searchResult[0];
        resultAddr = item["_result"];
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
    else
    {
      return SC_RESULT_ERROR_INVALID_STATE;
    }
  }

  return SC_RESULT_ERROR_INVALID_STATE;
}

} // namespace nl
