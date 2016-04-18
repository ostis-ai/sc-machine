/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "iotTypes.hpp"
#include "wrap/kpm/sc_agent.hpp"

#include "iotActions.generated.hpp"

namespace iot
{
	class ANewPeriodicalActionAgent : public ScAgent
	{
		SC_CLASS(Agent, Event(msActionPeriodical, SC_EVENT_ADD_OUTPUT_ARC))
		SC_GENERATED_BODY()

	private:
		SC_PROPERTY(Keynode, SysIdtf("action_periodical"))
		static ScAddr msActionPeriodical;
	};
}
