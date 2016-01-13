/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotAbout.hpp"
#include "iotKeynodes.hpp"
#include "iotUtils.hpp"

#include "wrap/sc_memory.hpp"

namespace iot
{

	IMPLEMENT_AGENT(WhoAreYou, COMMAND_AGENT)
	{
		return SC_RESULT_ERROR;
	}

	sc_result handler_who_are_you_command(sc_event const * event, sc_addr arg)
	{
		RUN_AGENT(WhoAreYou, Keynodes::command_who_are_you, sc_access_lvl_make_min, sc::Addr(arg));
	}
}