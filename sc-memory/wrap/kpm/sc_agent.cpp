/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_agent.hpp"

bool gInitializeResult = false;
bool gIsInitialized = false;

namespace Keynodes
{
    ScAddr command_in_progress;
    ScAddr nrel_result;
    ScAddr command_finished;

    ScAddr question_in_progress;
    ScAddr nrel_answer;
    ScAddr question_finished;
}

bool _resolveKeynodeImpl(ScMemoryContext & ctx, char const * str, ScAddr & outAddr)
{
    check_expr(ctx.isValid());

    // check if node with system identifier already exists
    if (ctx.helperResolveSystemIdtf(str, outAddr))
        return true;

    // create new node with specified system identifier
    outAddr = ctx.createNode(sc_type_node);
    if (outAddr.isValid())
        return ctx.helperSetSystemIdtf(str, outAddr);

    return false;
}

bool ScAgentInit()
{
    if (!gIsInitialized)
    {
        gIsInitialized = true;

        ScMemoryContext ctx(sc_access_lvl_make_max, "AgentKeynodes");

        gInitializeResult = true;
        gInitializeResult = gInitializeResult && _resolveKeynodeImpl(ctx, "command_in_progress", Keynodes::command_in_progress);
        gInitializeResult = gInitializeResult && _resolveKeynodeImpl(ctx, "command_finished", Keynodes::command_finished);
        gInitializeResult = gInitializeResult && _resolveKeynodeImpl(ctx, "nrel_result", Keynodes::nrel_result);

        gInitializeResult = gInitializeResult && _resolveKeynodeImpl(ctx, "question_finished", Keynodes::question_finished);
        gInitializeResult = gInitializeResult && _resolveKeynodeImpl(ctx, "question_in_progress", Keynodes::question_in_progress);
    }

    return gInitializeResult;
}

template<> sc_result ScAgent<KPM_COMMAND_AGENT>::run(ScAddr const & startArcAddr)
{
    ScAddr cmdAddr = mMemoryCtx.getArcEnd(startArcAddr);
	if (cmdAddr.isValid())
	{
		if (mMemoryCtx.helperCheckArc(mCmdClassAddr, cmdAddr, sc_type_arc_pos_const_perm))
		{
			mMemoryCtx.eraseElement(startArcAddr);
            ScAddr progressAddr = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::command_in_progress, cmdAddr);
			assert(progressAddr.isValid());
            ScAddr resultAddr = mMemoryCtx.createNode(sc_type_const | sc_type_node_struct);
			assert(resultAddr.isValid());

			runImpl(cmdAddr, resultAddr);

			mMemoryCtx.eraseElement(progressAddr);

            ScAddr const commonArc = mMemoryCtx.createArc(sc_type_const | sc_type_arc_common, cmdAddr, resultAddr);
			assert(commonArc.isValid());
            ScAddr const arc = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::nrel_result, commonArc);
			assert(arc.isValid());

			mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::command_finished, cmdAddr);

			return SC_RESULT_OK;
		}
	}

	return SC_RESULT_ERROR;
}

template<> sc_result ScAgent<KPM_QUESTION_AGENT>::run(ScAddr const & startArcAddr)
{
    ScAddr cmdAddr = mMemoryCtx.getArcEnd(startArcAddr);
	if (cmdAddr.isValid())
	{
		if (mMemoryCtx.helperCheckArc(mCmdClassAddr, cmdAddr, sc_type_arc_pos_const_perm))
		{
			mMemoryCtx.eraseElement(startArcAddr);
            ScAddr progressAddr = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::question_in_progress, cmdAddr);
			assert(progressAddr.isValid());
            ScAddr resultAddr = mMemoryCtx.createNode(sc_type_const | sc_type_node_struct);
			assert(resultAddr.isValid());

			runImpl(cmdAddr, resultAddr);

			mMemoryCtx.eraseElement(progressAddr);

            ScAddr const commonArc = mMemoryCtx.createArc(sc_type_const | sc_type_arc_common, cmdAddr, resultAddr);
			assert(commonArc.isValid());
            ScAddr const arc = mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::nrel_answer, commonArc);
			assert(arc.isValid());

			mMemoryCtx.createArc(sc_type_arc_pos_const_perm, Keynodes::question_finished, cmdAddr);

			return SC_RESULT_OK;
		}
	}

	return SC_RESULT_ERROR;
}
