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
        ScIterator5Ptr itCmd = mMemoryCtx.iterator5(
			requestAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_node | sc_type_const),
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::rrel_1
			);

		if (!itCmd->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;

		// got command addr
        ScAddr commandAddr;
        ScAddr const commandInstAddr = itCmd->value(2);
		ScIterator3Ptr itCommandClass = mMemoryCtx.iterator3(
			SC_TYPE(sc_type_node | sc_type_const | sc_type_node_class),
			SC_TYPE(sc_type_arc_pos_const_perm),
			commandInstAddr);
		while (itCommandClass->next())
		{
			if (mMemoryCtx.helperCheckArc(Keynodes::command, itCommandClass->value(0), SC_TYPE(sc_type_arc_pos_const_perm)))
			{
				commandAddr = itCommandClass->value(0);
				break;
			}
		}

		if (!commandAddr.isValid())
			return SC_RESULT_ERROR_INVALID_STATE;

        ScIterator5Ptr itLang = mMemoryCtx.iterator5(
			requestAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_node | sc_type_const),
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::rrel_2
			);
		if (!itLang->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;

        ScAddr const langAddr = itLang->value(2);

		ScIterator5Ptr itAttr = mMemoryCtx.iterator5(
			requestAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_node | sc_type_const),
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::rrel_3
			);
		if (!itAttr->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;
        ScAddr const attrAddr = itAttr->value(2);

		/// TODO: make commond method to get arguments with custom role

		// check if there are speech templates for a specified command
        ScIterator5Ptr itTemplatesSet = mMemoryCtx.iterator5(
			SC_TYPE(sc_type_node | sc_type_const | sc_type_node_tuple),
			SC_TYPE(sc_type_arc_common | sc_type_const),
			commandAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			Keynodes::nrel_speech_templates
			);

		
		if (!itTemplatesSet->next())
			return SC_RESULT_ERROR_INVALID_PARAMS;
		// got templates set
        ScAddr const templatesAddr = itTemplatesSet->value(0);
		
		// try to find template for a specified language
        ScIterator5Ptr itTempl = mMemoryCtx.iterator5(
			templatesAddr,
			SC_TYPE(sc_type_arc_pos_const_perm),
			SC_TYPE(sc_type_link),
			SC_TYPE(sc_type_arc_pos_const_perm),
			attrAddr
			);

		/// TODO: possible select random template from a set (more then one template for language and result attr)
		while (itTempl->next())
		{
            ScAddr const linkAddr = itTempl->value(2);
			if (mMemoryCtx.helperCheckArc(langAddr, linkAddr, sc_type_arc_pos_const_perm))
			{
                ScStream stream;
				if (mMemoryCtx.getLinkContent(linkAddr, stream))
				{
					std::string strTemplate;
					if (StreamConverter::streamToString(stream, strTemplate))
					{
						std::string resultText;
						TextTemplateProcessor processor(mMemoryCtx, strTemplate, langAddr);
						if (processor.generateOutputText(resultText))
						{
                            ScAddr const resultLink = mMemoryCtx.createLink();
							assert(resultLink.isValid());

                            ScStream resultStream(resultText.c_str(), (sc_uint32)resultText.size(), SC_STREAM_FLAG_READ | SC_STREAM_FLAG_SEEK);
							
							bool const res = mMemoryCtx.setLinkContent(resultLink, resultStream);
							assert(res);

                            ScAddr const edge = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, resultAddr, resultLink);
							assert(edge.isValid());
							
						}						
					}
				}
				else
				{
					/// TODO: generate default text

					// for a fast test, just use template as an answer
                    ScAddr const edge = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, resultAddr, linkAddr);
					assert(edge.isValid());
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
		if (mMemoryCtx.helperFindBySystemIdtf(arguments, elAddr))
		{
            ScAddr linkIdtf = Utils::findMainIdtf(mMemoryCtx, elAddr, mLanguageAddr);
			if (linkIdtf.isValid())
			{
                ScStream stream;
				if (mMemoryCtx.getLinkContent(linkIdtf, stream))
				{
					StreamConverter::streamToString(stream, result);
				}
			}
		}
		return result;
	}

}