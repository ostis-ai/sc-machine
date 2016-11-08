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
        ScAddr dialogueAddr, itemAddr;
        check_expr(dialogueAddr == Keynodes::msMainNLDialogueInstance);
        mMemoryCtx.getEdgeInfo(argAddr, dialogueAddr, itemAddr);

        if (!mMemoryCtx.getElementType(itemAddr).isLink())
            return SC_RESULT_ERROR_INVALID_PARAMS;

        if (itemAddr.isValid())
        {
            /*
             * command_process_user_text_message
             * _<- _command_instance
             * (* _-> _message;; *);;
             */
            ScTemplate cmdProcessTextTemplate;
            cmdProcessTextTemplate
                (Keynodes::msCmdProcessUserTextMessage,
                 ScType::EDGE_ACCESS_VAR_POS_PERM,
                 ScType::NODE_CONST >> "_command_instance")
                ("_command_instance",
                 ScType::EDGE_ACCESS_VAR_POS_PERM,
                 itemAddr);

            ScTemplateGenResult result;
            if (mMemoryCtx.helperGenTemplate(cmdProcessTextTemplate, result))
            {
                const ScAddr cmdAddr = result["_command_instance"];
                check_expr(cmdAddr.isValid());

                // initiate command
                mMemoryCtx.createEdge(ScType::EDGE_ACCESS_CONST_POS_PERM, Keynodes::msCommandInitiated, cmdAddr);
                
                // wait until command finish
                ScWaitCondition<ScEventAddInputEdge> waiter(mMemoryCtx, cmdAddr, [this](const ScAddr & addr, const ScAddr & argAddr){
                    ScAddr sourceAddr, targetAddr;
                    if (mMemoryCtx.getEdgeInfo(argAddr, sourceAddr, targetAddr))
                        return (targetAddr == Keynodes::msCommandFinished);

                    return false;
                });

                if (!waiter.Wait())
                    return SC_RESULT_ERROR_INVALID_STATE;

                // parse result and append it into dialogue
                ScTemplate resultTemplate;
                resultTemplate
                    (cmdAddr,
                     ScType::EDGE_DCOMMON_VAR,
                     ScType::NODE_VAR_STRUCT >> "_result",
                     ScType::EDGE_ACCESS_VAR_POS_PERM,
                     Keynodes::msNrelResult);

                ScTemplateSearchResult searchResult;
                if (!mMemoryCtx.helperSearchTemplate(resultTemplate, searchResult))
                    return SC_RESULT_ERROR_INVALID_STATE;

                ScAddr resultAddr;
                {
                    ScTemplateSearchResultItem item = searchResult[0];
                    resultAddr = item["_result"];
                }
                
                /// TODO: add blockers, to prevent race conditions with last element
                // append it into dialogue
                ScTemplate lastItemTemplate;
                lastItemTemplate
                    (Keynodes::msMainNLDialogueInstance,
                     ScType::EDGE_ACCESS_VAR_POS_PERM,
                     ScType::NODE_VAR >> "_last_item",
                     ScType::EDGE_ACCESS_VAR_POS_PERM >> "_last_item_attr",
                     Keynodes::msRrelLastItem);

                ScAddr lastItemAddr;
                if (mMemoryCtx.helperSearchTemplate(lastItemTemplate, searchResult))
                {
                    ScTemplateSearchResultItem item = searchResult[0];
                    lastItemAddr = item["_last_item"];
                    // remove attr edge
                    mMemoryCtx.eraseElement(item["_last_item_attr"]);
                }
                
                // if previous item exist, then append item order relation
                if (lastItemAddr.isValid())
                {
                    ScTemplate nextItemTemplate;
                    nextItemTemplate
                        (lastItemAddr,
                        ScType::EDGE_DCOMMON_VAR,
                        resultAddr,
                        ScType::EDGE_ACCESS_VAR_POS_PERM,
                        Keynodes::msNrelItemOrder);

                    ScTemplateGenResult genResult;
                    if (!mMemoryCtx.helperGenTemplate(nextItemTemplate, genResult))
                        return SC_RESULT_ERROR_INVALID_STATE;
                }

                // append last item attr
                {
                    ScTemplateGenParams params;
                    params.add("_last_item", resultAddr);
                    
                    ScTemplateGenResult genResult;
                    if (!mMemoryCtx.helperGenTemplate(lastItemTemplate, genResult, params))
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
}