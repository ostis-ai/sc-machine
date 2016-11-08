/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "iotTypes.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "iotSpeech.generated.hpp"

namespace iot
{

	class AGenerateTextByTemplate : public ScAgentAction
	{
		SC_CLASS(Agent, CmdClass("command_generate_text_from_template"))
		SC_GENERATED_BODY()
	};


	class TextTemplateProcessor
	{
	public:
        explicit TextTemplateProcessor(ScMemoryContext & memoryCtx, std::string const & str, ScAddr const & langAddr);
		virtual ~TextTemplateProcessor();

		bool generateOutputText(std::string & outText);

	protected:
		std::string processMainIdtfCmd(std::string & arguments);

	private:
        ScMemoryContext & mMemoryCtx;
		std::string mInputTextTemplate;
        ScAddr mLanguageAddr;
	};

}
