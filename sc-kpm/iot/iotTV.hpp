/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "iotTypes.hpp"
#include "sc-memory/cpp/kpm/sc_agent.hpp"

#include "iotTV.generated.hpp"

namespace iot
{

	class ATVChangeProgram : public ScAgentAction
	{
		SC_CLASS(Agent, CmdClass("command_change_tv_program"))
		SC_GENERATED_BODY()
	};

}
