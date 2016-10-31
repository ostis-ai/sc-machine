/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotKeynodes.hpp"
#include "iotUtils.hpp"

#include "sc-memory/cpp/sc_memory.hpp"

#include "iotTV.hpp"

namespace iot
{
	SC_AGENT_ACTION_IMPLEMENTATION(ATVChangeProgram)
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
			ScAddr edge = mMemoryCtx.createEdge(SC_TYPE(sc_type_arc_common | sc_type_const), tvAddr, programAddr);
			if (!edge.isValid())
				return SC_RESULT_ERROR;

			ScAddr edgeRel = mMemoryCtx.createEdge(SC_TYPE(sc_type_arc_pos_const_perm), Keynodes::nrel_tv_program, edge);
			if (!edgeRel.isValid())
				return SC_RESULT_ERROR;
		}

		return SC_RESULT_OK;
	}

}