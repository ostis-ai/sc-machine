/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotAgent.hpp"
#include "iotKeynodes.hpp"

namespace iot
{
	Agent::Agent(sc::Addr const & cmdClassAddr, char const * name, sc_uint8 accessLvl)
		: mCmdClassAddr(cmdClassAddr)
		, mMemoryCtx(accessLvl, name)
	{
		
	}

	Agent::~Agent()
	{
	}

	sc_result Agent::run(sc::Addr const & startArcAddr)
	{
		sc::Addr cmdAddr = mMemoryCtx.getArcEnd(startArcAddr);
		if (cmdAddr.isValid())
		{
			if (mMemoryCtx.helperCheckArc(mCmdClassAddr, cmdAddr, sc_type_arc_pos_const_perm))
			{
				mMemoryCtx.eraseElement(startArcAddr);
				sc::Addr progressAddr = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::command_in_progress, cmdAddr);

				runImpl(cmdAddr);

				mMemoryCtx.eraseElement(progressAddr);
				mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::command_finished, cmdAddr);

				return SC_RESULT_OK;
			}
		}

		return SC_RESULT_ERROR;
	}

}