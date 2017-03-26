/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "nlTextTemplProcessor.hpp"
#include "nlTextTemplProcessorTokens.hpp"

#include <sc-memory/cpp/sc_stream.hpp>


namespace nl
{
namespace util
{

ScAddr TextTemplProcessor::ms_nrelMainIdtf;

TextTemplProcessor::TextTemplProcessor(ScMemoryContext & ctx)
  : m_memoryCtx(ctx)
{

}

sc_result TextTemplProcessor::operator() (ScAddr const & textTempl,
                                          ScAddr const & lang,
                                          ScAddr const & inputData,
                                          ScAddr const & parseTempl)
{
  SC_ASSERT(textTempl.IsValid(), ("Invalid sc-addr to text template"));
  
  // get input text
  ScStream stream;
  if (!m_memoryCtx.GetLinkContent(textTempl, stream))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  std::string textTemplStr;
  if (!ScStreamConverter::StreamToString(stream, textTemplStr))
    return SC_RESULT_ERROR;

  // now need to parse input data, if it exist
  if (inputData.IsValid() && parseTempl.IsValid())
  {
    // TODO: prepare parameters
  }
  else if (inputData.IsValid() || parseTempl.IsValid()) 
  {
    // should be used together
    return SC_RESULT_ERROR_INVALID_STATE;
  }

  // generate tokens
  Tokenizer tokenize(lang);
  tokenize(textTemplStr);

  TokensRender render;
  m_result = render(tokenize.GetTokens(), m_memoryCtx);

  return SC_RESULT_OK;
}

} // namespace util
} // namespace nl