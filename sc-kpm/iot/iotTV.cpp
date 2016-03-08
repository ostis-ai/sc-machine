/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotTV.hpp"
#include "iotKeynodes.hpp"
#include "iotUtils.hpp"

#include "wrap/sc_memory.hpp"

namespace iot
{
	IMPLEMENT_AGENT(TVChangeProgram, COMMAND_AGENT)
	{
		// get tv device
        ScAddr tvAddr = getParam(requestAddr, Keynodes::rrel_1, 0);
        ScAddr programAddr = getParam(requestAddr, Keynodes::rrel_2, 0);

		if (!tvAddr.isValid() || !programAddr.isValid())
			return SC_RESULT_ERROR_INVALID_PARAMS;

		// remove old program relation
		{
			ScIterator5Ptr iter = mMemoryCtx.iterator5(tvAddr,
				SC_TYPE(sc_type_arc_common | sc_type_const),
				SC_TYPE(sc_type_node | sc_type_const),
				SC_TYPE(sc_type_arc_pos_const_perm),
				Keynodes::nrel_tv_program);

			if (iter->next())
				mMemoryCtx.eraseElement(iter->value(2));
		}

		// create link to new program
		{
            ScAddr edge = mMemoryCtx.createArc(SC_TYPE(sc_type_arc_common | sc_type_const), tvAddr, programAddr);
			if (!edge.isValid())
				return SC_RESULT_ERROR;

            ScAddr edgeRel = mMemoryCtx.createArc(SC_TYPE(sc_type_arc_pos_const_perm), Keynodes::nrel_tv_program, edge);
			if (!edgeRel.isValid())
				return SC_RESULT_ERROR;
		}

		return SC_RESULT_OK;
	}

	sc_result handler_change_tv_program(sc_event const * event, sc_addr arg)
	{
        RUN_AGENT(TVChangeProgram, Keynodes::command_change_tv_program, sc_access_lvl_make_min, ScAddr(arg));
	}
}