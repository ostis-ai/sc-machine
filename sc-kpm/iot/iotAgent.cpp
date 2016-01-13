/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotAgent.hpp"
#include "iotKeynodes.hpp"

namespace iot
{
	

	template<> sc_result Agent<COMMAND_AGENT>::run(sc::Addr const & startArcAddr)
	{
		sc::Addr cmdAddr = mMemoryCtx.getArcEnd(startArcAddr);
		if (cmdAddr.isValid())
		{
			if (mMemoryCtx.helperCheckArc(mCmdClassAddr, cmdAddr, sc_type_arc_pos_const_perm))
			{
				mMemoryCtx.eraseElement(startArcAddr);
				sc::Addr progressAddr = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::command_in_progress, cmdAddr);
				assert(progressAddr.isValid());
				sc::Addr resultAddr = mMemoryCtx.createNode(sc_type_const | sc_type_node_struct);
				assert(resultAddr.isValid());

				runImpl(cmdAddr, resultAddr);

				mMemoryCtx.eraseElement(progressAddr);

				sc::Addr const commonArc = mMemoryCtx.createArc(sc_type_const | sc_type_arc_common, cmdAddr, resultAddr);
				assert(commonArc.isValid());
				sc::Addr const arc = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::nrel_result, commonArc);
				assert(arc.isValid());

				mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::command_finished, cmdAddr);

				return SC_RESULT_OK;
			}
		}

		return SC_RESULT_ERROR;
	}

	template<> sc_result Agent<QUESTION_AGENT>::run(sc::Addr const & startArcAddr)
	{
		sc::Addr cmdAddr = mMemoryCtx.getArcEnd(startArcAddr);
		if (cmdAddr.isValid())
		{
			if (mMemoryCtx.helperCheckArc(mCmdClassAddr, cmdAddr, sc_type_arc_pos_const_perm))
			{
				mMemoryCtx.eraseElement(startArcAddr);
				sc::Addr progressAddr = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::question_in_progress, cmdAddr);
				assert(progressAddr.isValid());
				sc::Addr resultAddr = mMemoryCtx.createNode(sc_type_const | sc_type_node_struct);
				assert(resultAddr.isValid());

				runImpl(cmdAddr, resultAddr);

				mMemoryCtx.eraseElement(progressAddr);

				sc::Addr const commonArc = mMemoryCtx.createArc(sc_type_const | sc_type_arc_common, cmdAddr, resultAddr);
				assert(commonArc.isValid());
				sc::Addr const arc = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::nrel_answer, commonArc);
				assert(arc.isValid());

				mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::question_finished, cmdAddr);

				return SC_RESULT_OK;
			}
		}

		return SC_RESULT_ERROR;
	}

}