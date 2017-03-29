/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nlGenCmdTextResult.hpp"

#include <sc-memory/cpp/sc_wait.hpp>

namespace nl
{

ScAddr AGenCmdTextResult::ms_kLangEn;
ScAddr AGenCmdTextResult::ms_kCommand;
ScAddr AGenCmdTextResult::ms_kNrelTextTemplates;
ScAddr AGenCmdTextResult::ms_kNrelSuitableFor;
ScAddr AGenCmdTextResult::ms_kCommandGetTextTempl;

SC_AGENT_ACTION_IMPLEMENTATION(AGenCmdTextResult)
{
  // get command instance to process it
  ScAddr const cmdInstance = GetParam(requestAddr, 0);
  if (!cmdInstance.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // determine command class
  ScAddr cmdClass;
  {
    ScTemplate templ;
    templ.Triple(
      ScType::NodeVarClass >> "_cmdClass",
      ScType::EdgeAccessVarPosPerm,
      cmdInstance
    );

    SC_ASSERT(ms_kCommand.IsValid(), ());
    templ.Triple(
      ms_kCommand,
      ScType::EdgeAccessVarPosPerm,
      "_cmdClass"
    );

    ScTemplateSearchResult res;
    if (!m_memoryCtx.HelperSearchTemplate(templ, res))
      return SC_RESULT_ERROR_INVALID_PARAMS;

    if (res.Size() > 1)
      return SC_RESULT_ERROR;

    cmdClass = res[0]["_cmdClass"];
  }

  SC_ASSERT(cmdClass.IsValid(), ());

  // get command result
  ScAddr const cmdResult = ScAgentAction::GetCommandResultCodeAddr(m_memoryCtx, cmdInstance);
  if (!cmdResult.IsValid())
    return SC_RESULT_ERROR;

  // find suitable templates set depending on a result
  ScAddr templateCandidatesSet;
  {
    ScTemplate templ;
    templ.TripleWithRelation(
      ScType::NodeVarTuple >> "_set",
      ScType::EdgeDCommonVar,
      cmdClass,
      ScType::EdgeAccessVarPosPerm,
      ms_kNrelTextTemplates
    );

    templ.Triple(
      "_set",
      ScType::EdgeAccessVarPosPerm,
      ScType::NodeVarTuple >> "_candSet"
    );

    templ.TripleWithRelation(
      "_candSet",
      ScType::EdgeDCommonVar,
      cmdResult,
      ScType::EdgeAccessVarPosPerm,
      ms_kNrelSuitableFor
    );

    ScTemplateSearchResult res;
    /// TODO: support usage of common sc_result_error, if sc_result_error_.. wasn't found
    if (!m_memoryCtx.HelperSearchTemplate(templ, res) || (res.Size() != 1))
      return SC_RESULT_ERROR_INVALID_STATE;

    templateCandidatesSet = res[0]["_candSet"];
  }

  SC_ASSERT(templateCandidatesSet.IsValid(), ());

  /// TODO: support author language
  ScAddr const lang = ms_kLangEn;

  // find all suitable candidates for a language
  std::vector<ScAddr> candidates;
  candidates.reserve(128);
  {
    ScTemplate templ;
    templ.Triple(
      templateCandidatesSet,
      ScType::EdgeAccessVarPosPerm,
      ScType::Link >> "_templ"
    );

    templ.Triple(
      lang,
      ScType::EdgeAccessVarPosPerm,
      "_templ"
    );

    ScTemplateSearchResult res;
    if (!m_memoryCtx.HelperSearchTemplate(templ, res))
      return SC_RESULT_ERROR_NOT_FOUND;

    res.ForEach([&candidates](ScTemplateSearchResultItem const & item)
    {
      candidates.push_back(item["_templ"]);
    });
  }

  SC_ASSERT(!candidates.empty(), ());

  // now get random one
  ScAddr const templateAddr = candidates[utils::Random::Int() % candidates.size()];

  // run agent to generate output text and wait result from it
  ScAddr resultText;
  {
    /// TODO: add support of parameter templates
    ScAddr const genCmd = ScAgentAction::CreateCommand(m_memoryCtx, ms_kCommandGetTextTempl, { templateAddr, lang });
    ScWaitActionFinished waiter(m_memoryCtx, genCmd);
    waiter.SetOnWaitStartDelegate([&genCmd, this]()
    {
      ScAgentAction::InitiateCommand(m_memoryCtx, genCmd);
    });
    if (!waiter.Wait())
      return SC_RESULT_ERROR;

    ScAddr const res = ScAgentAction::GetCommandResultAddr(m_memoryCtx, genCmd);
    if (!res.IsValid())
      return SC_RESULT_ERROR;

    ScIterator3Ptr it = m_memoryCtx.Iterator3(
      res,
      ScType::EdgeAccessConstPosPerm,
      ScType::Link);

    if (!it->Next())
        return SC_RESULT_ERROR;
    resultText = it->Get(2);
    SC_ASSERT(!it->Next(), ("Invalid state of result"));
  }
  
  ScAddr const edge = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, resultAddr, resultText);
  
  return edge.IsValid() ? SC_RESULT_OK : SC_RESULT_ERROR;
}

} // namespace nl
