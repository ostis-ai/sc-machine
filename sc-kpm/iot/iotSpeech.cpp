/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotKeynodes.hpp"
#include "iotUtils.hpp"

#include "sc-memory/cpp/sc_memory.hpp"
#include "sc-memory/cpp/sc_stream.hpp"

#include "iotSpeech.hpp"

namespace iot
{

SC_AGENT_ACTION_IMPLEMENTATION(AGenerateTextByTemplate)
{
  ScAddr const commandInstAddr = GetParam(requestAddr, Keynodes::rrel_1, ScType::NodeConst);
  if (!commandInstAddr.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // got command addr
  ScAddr commandAddr;
  ScIterator3Ptr itCommandClass = m_memoryCtx.Iterator3(
        ScType::NodeConstClass,
        ScType::EdgeAccessConstPosPerm,
        commandInstAddr);

  while (itCommandClass->Next())
  {
    if (m_memoryCtx.HelperCheckEdge(Keynodes::command, itCommandClass->Get(0), ScType::EdgeAccessConstPosPerm))
    {
      commandAddr = itCommandClass->Get(0);
      break;
    }
  }

  if (!commandAddr.IsValid())
    return SC_RESULT_ERROR_INVALID_STATE;

  
  ScAddr const langAddr = GetParam(requestAddr, Keynodes::rrel_2, ScType::NodeConst);
  if (!langAddr.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;
  
  ScAddr const attrAddr = GetParam(requestAddr, Keynodes::rrel_3, ScType::NodeConst);
  if (!attrAddr.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  /// TODO: make commond method to get arguments with custom role

  // check if there are speech templates for a specified command
  ScIterator5Ptr itTemplatesSet = m_memoryCtx.Iterator5(
        ScType::NodeConstTuple,
        ScType::EdgeDCommonConst,
        commandAddr,
        ScType::EdgeAccessConstPosPerm,
        Keynodes::nrel_speech_templates);

  if (!itTemplatesSet->Next())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // got templates set
  ScAddr const templatesAddr = itTemplatesSet->Get(0);

  // try to find template for a specified language
  ScIterator5Ptr itTempl = m_memoryCtx.Iterator5(
        templatesAddr,
        ScType::EdgeAccessConstPosPerm,
        ScType::Link,
        ScType::EdgeAccessConstPosPerm,
        attrAddr);

  /// TODO: possible select random template from a set (more then one template for language and result attr)
  while (itTempl->Next())
  {
    ScAddr const linkAddr = itTempl->Get(2);
    if (m_memoryCtx.HelperCheckEdge(langAddr, linkAddr, ScType::EdgeAccessConstPosPerm))
    {
      ScStream stream;
      if (m_memoryCtx.GetLinkContent(linkAddr, stream))
      {
        std::string strTemplate;
        if (ScStreamConverter::StreamToString(stream, strTemplate))
        {
          std::string resultText;
          TextTemplateProcessor processor(m_memoryCtx, strTemplate, langAddr);
          if (processor.generateOutputText(resultText))
          {
            ScAddr const resultLink = m_memoryCtx.CreateLink();
            SC_ASSERT(resultLink.IsValid(), ());

            ScStream resultStream(resultText.c_str(), (sc_uint32)resultText.size(), SC_STREAM_FLAG_READ | SC_STREAM_FLAG_SEEK);

            m_memoryCtx.SetLinkContent(resultLink, resultStream);
            m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, resultAddr, resultLink);
          }
        }
      }
      else
      {
        /// TODO: generate default text

        // for a fast test, just use template as an answer
        m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, resultAddr, linkAddr);
      }

      return SC_RESULT_OK;
    }
  }


  return SC_RESULT_ERROR;
}


// ----------------- Template processor ---------------

TextTemplateProcessor::TextTemplateProcessor(ScMemoryContext & memoryCtx, std::string const & str, ScAddr const & langAddr)
  : mMemoryCtx(memoryCtx)
  , mInputTextTemplate(str)
  , mLanguageAddr(langAddr)
{
}

TextTemplateProcessor::~TextTemplateProcessor()
{
}

bool TextTemplateProcessor::generateOutputText(std::string & outText)
{
  /// TODO: make complex template language parser

  // for that moment we will parse just on command ($main_idtf)
  // syntax: $main_idtf(<sysIdtf>);
  size_t currentChar = 0, prevChar = 0;
  while (1)
  {
    currentChar = mInputTextTemplate.find_first_of("$", currentChar);
    if (currentChar == std::string::npos)
      break;

    outText += mInputTextTemplate.substr(prevChar, currentChar - prevChar);

    // determine command name
    size_t bracketStart = mInputTextTemplate.find_first_of("(", currentChar);
    if (bracketStart != std::string::npos)
    {
      std::string commandName = mInputTextTemplate.substr(currentChar + 1, bracketStart - currentChar - 1);

      // determine arguments end
      size_t bracketEnd = mInputTextTemplate.find_first_of(")", bracketStart);
      if (bracketEnd != std::string::npos)
      {
        prevChar = bracketEnd;
        std::string arguments = mInputTextTemplate.substr(bracketStart + 1, bracketEnd - bracketStart - 1);

        /// TODO: parse arguments list

        std::string replacement;
        if (commandName == "main_idtf")
        {
          replacement = processMainIdtfCmd(arguments);
        }

        // replace command by result value
        outText += replacement;
      }
    }

    ++currentChar;
  };

  if (currentChar != std::string::npos)
    outText += mInputTextTemplate.substr(prevChar);

  return true;
}


std::string TextTemplateProcessor::processMainIdtfCmd(std::string & arguments)
{
  std::string result;
  ScAddr elAddr;
  if (mMemoryCtx.HelperFindBySystemIdtf(arguments, elAddr))
  {
    ScAddr linkIdtf = utils::findMainIdtf(mMemoryCtx, elAddr, mLanguageAddr);
    if (linkIdtf.IsValid())
    {
      ScStream stream;
      if (mMemoryCtx.GetLinkContent(linkIdtf, stream))
        ScStreamConverter::StreamToString(stream, result);
    }
  }
  return result;
}

} // namespace iot
