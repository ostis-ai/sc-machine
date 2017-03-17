/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nlTextTemplProcessor.hpp"

#include "nlGenText.hpp"

#include <sc-memory/cpp/sc_stream.hpp>

namespace nl
{

SC_AGENT_ACTION_IMPLEMENTATION(AGenText)
{
  // first of all we need to parse input parameters
  ScAddr const textTemplate = GetParam(requestAddr, 0);
  ScAddr const lang = GetParam(requestAddr, 1);
  ScAddr const inputData = GetParam(requestAddr, 2);
  ScAddr const parseTempl = GetParam(requestAddr, 3);

  if (!textTemplate.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  if (!lang.IsValid())
    SC_NOT_IMPLEMENTED("Support of empty language parameter");

  nl::util::TextTemplProcessor textProc(m_memoryCtx);
  sc_result const res = textProc(textTemplate, lang, inputData, parseTempl);
  if (res != SC_RESULT_OK)
    return res;

  std::string const & resultStr = textProc.GetResult();

  // generate result
  ScAddr const resultTextAddr = m_memoryCtx.CreateLink();
  ScStream stream(resultStr.c_str(), resultStr.size(), SC_STREAM_FLAG_READ);

  if (resultStr.empty() || !m_memoryCtx.SetLinkContent(resultTextAddr, stream))
    return SC_RESULT_ERROR;

  if (!m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, resultAddr, resultTextAddr).IsValid())
    return SC_RESULT_ERROR;

  return SC_RESULT_OK;
}

} // namespace nl
