/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "wrap/kpm/sc_agent.hpp"
#include "nlIvona.generated.hpp"

namespace nl
{
	class AIvonaGenerateSpeechAgent : public ScAgentAction
	{
		SC_CLASS(Agent, CmdClass("command_generate_speech_from_text"))
		SC_GENERATED_BODY()

	private:
		SC_PROPERTY(Keynode("format_ogg"), ForceCreation)
		static ScAddr msFormatOgg;

		SC_PROPERTY(Keynode("nrel_format"), ForceCreation)
		static ScAddr msNrelFormat;
	};

	void runTest();
}