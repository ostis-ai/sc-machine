/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nlApiAi.hpp"

#include "sc-memory/cpp/sc_stream.hpp"
#include "sc-memory/cpp/sc_struct.hpp"

namespace nl
{

ScAddr AApiAiParseUserTextAgent::ms_nrelCommonTemplate;
ScAddr AApiAiParseUserTextAgent::ms_nrelTranslation;
ScAddr AApiAiParseUserTextAgent::ms_rrelLocation;

sc_result AApiAiParseUserTextAgent::TryCommonTemplate(ScAddr const & actionAddr,
                                                      ScAddr const & resultAddr,
                                                      ApiAiRequestResult const & resultParsed)
{
  ScIterator5Ptr iterTempl = m_memoryCtx.Iterator5(
    actionAddr,
    ScType::EdgeDCommonConst,
    ScType::NodeConstStruct,
    ScType::EdgeAccessConstPosPerm,
    ms_nrelCommonTemplate);

  if (iterTempl->Next())
  {
    ScTemplate templ;
    if (m_memoryCtx.HelperBuildTemplate(templ, iterTempl->Get(2)))
    {
      // setup template parameters
      ScTemplateGenParams params;

      if (templ.HasReplacement("_apiai_location"))
      {
        /// TODO: replace location by real city addr, or something else
      }

      if (templ.HasReplacement("_lang"))
      {
        /// TOOD: replace with current user language
      }

      if (templ.HasReplacement("_apiai_speech"))
      {
        ScAddr const speechAddr = m_memoryCtx.CreateLink();

        std::string const & speech = resultParsed.GetFullfillment().getSpeech();
        ScStream stream(speech.c_str(), (uint32_t)speech.size(), SC_STREAM_FLAG_READ);
        if (!m_memoryCtx.SetLinkContent(speechAddr, stream))
          return SC_RESULT_ERROR_IO;

        params.add("_apiai_speech", speechAddr);
      }

      params.add("_result", resultAddr);

      // generate with params
      ScTemplateGenResult genResult;
      return m_memoryCtx.HelperGenTemplate(templ, genResult, params) ? SC_RESULT_OK : SC_RESULT_ERROR;
    }
  }

  return SC_RESULT_NO;
}

sc_result AApiAiParseUserTextAgent::TryCommandEmit(ScAddr const & actionAddr,
                                                   ScAddr const & resultAddr,
                                                   ApiAiRequestResult const & resultParsed)
{
  return SC_RESULT_NO;
}

/**
  * Command template: requestAddr _-> _text;;
  */
SC_AGENT_ACTION_IMPLEMENTATION(AApiAiParseUserTextAgent)
{
  curl_global_init(CURL_GLOBAL_ALL);

  // get text content of argument
  ScIterator3Ptr iter = m_memoryCtx.Iterator3(
    requestAddr,
    ScType::EdgeAccessConstPosPerm,
    ScType::Link);

  if (iter->Next())
  {
    ScStream stream;
    if (m_memoryCtx.GetLinkContent(iter->Get(2), stream))
    {
      std::string text;
      if (ScStreamConverter::StreamToString(stream, text))
      {
        ApiAiRequestParams params;
        params.m_queryString = text;
        params.m_sessionId = "test_session";	/// TODO: Build session id based on user node
        params.m_lang = "en";	/// TODO: get from kb

        ApiAiRequest request;
        if (request.Perform(params))
        {
          // try to find template for specified action
          ApiAiRequestResult const & result = request.GetResult();

          std::string actionName = utils::StringUtils::ReplaceAll(result.GetAction(), ".", "_");

          ScAddr actionAddr;
          if (m_memoryCtx.HelperFindBySystemIdtf(std::string("apiai_action_") + actionName, actionAddr))
          {
            // trying common template at first
            sc_result res = TryCommonTemplate(actionAddr, resultAddr, result);
            if (res != SC_RESULT_NO)
            {
              return res;
            }

            res = TryCommandEmit(actionAddr, resultAddr, result);
            if (res != SC_RESULT_NO)
              return res;
          }
        }
      }
      else
      {
        return SC_RESULT_ERROR_IO;
      }
    }
    else
    {
      return SC_RESULT_ERROR_INVALID_STATE;
    }
  }

  return SC_RESULT_ERROR;
}

} // namespace nl
