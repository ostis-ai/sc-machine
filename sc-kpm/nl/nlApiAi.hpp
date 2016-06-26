/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "wrap/kpm/sc_agent.hpp"
#include "nlApiAi.generated.hpp"

namespace nl
{
	class AApiAiParseUserTextAgent : public ScAgentAction
	{
		SC_CLASS(Agent, CmdClass("command_process_user_text_message"))
		SC_GENERATED_BODY()
	};
}