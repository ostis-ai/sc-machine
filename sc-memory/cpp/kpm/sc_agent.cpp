/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_agent.hpp"

bool gInitializeResult = false;
bool gIsInitialized = false;

bool _resolveKeynodeImpl(ScMemoryContext & ctx, char const * str, ScAddr & outAddr)
{
    check_expr(ctx.isValid());

    return ctx.helperResolveSystemIdtf(str, outAddr, true);
}

bool ScAgentInit()
{
    if (!gIsInitialized)
    {
		gInitializeResult = ScAgentAction::initGlobal();
		        
		gIsInitialized = true;
    }

    return gInitializeResult;
}


ScAgent::ScAgent(char const * name, sc_uint8 accessLvl)
	: mMemoryCtx(accessLvl, name)
{
}

ScAgent::~ScAgent()
{
}

sc_result ScAgent::run(ScAddr const & listedAddr, ScAddr const & argAddr)
{
	return SC_RESULT_ERROR;
}


// ---------------------------
ScAddr ScAgentAction::msCommandInitiatedAddr;
ScAddr ScAgentAction::msCommandProgressdAddr;
ScAddr ScAgentAction::msCommandFinishedAddr;
ScAddr ScAgentAction::msNrelResult;

ScAgentAction::ScAgentAction(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl)
	: ScAgent(name, accessLvl)
	, mCmdClassAddr(cmdClassAddr)
	
{
}

ScAgentAction::~ScAgentAction()
{
}

sc_result ScAgentAction::run(ScAddr const & listenAddr, ScAddr const & startArcAddr)
{
    ScAddr cmdAddr = mMemoryCtx.getEdgeTarget(startArcAddr);
	if (cmdAddr.isValid())
	{
		if (mMemoryCtx.helperCheckArc(mCmdClassAddr, cmdAddr, sc_type_arc_pos_const_perm))
		{
			mMemoryCtx.eraseElement(startArcAddr);
            ScAddr progressAddr = mMemoryCtx.createEdge(sc_type_arc_pos_const_perm, msCommandProgressdAddr, cmdAddr);
			assert(progressAddr.isValid());
            ScAddr resultAddr = mMemoryCtx.createNode(sc_type_const | sc_type_node_struct);
			assert(resultAddr.isValid());

			runImpl(cmdAddr, resultAddr);

			mMemoryCtx.eraseElement(progressAddr);

            ScAddr const commonArc = mMemoryCtx.createEdge(sc_type_const | sc_type_arc_common, cmdAddr, resultAddr);
			assert(commonArc.isValid());
			ScAddr const arc = mMemoryCtx.createEdge(sc_type_arc_pos_const_perm, msNrelResult, commonArc);
			assert(arc.isValid());

			mMemoryCtx.createEdge(sc_type_arc_pos_const_perm, msCommandFinishedAddr, cmdAddr);

			return SC_RESULT_OK;
		}
	}

	return SC_RESULT_ERROR;
}

ScAddr ScAgentAction::getParam(ScAddr const & cmdAddr, ScAddr const & relationAddr, sc_type paramType)
{
	ScIterator5Ptr iter = mMemoryCtx.iterator5(cmdAddr,
		SC_TYPE(sc_type_arc_pos_const_perm),
		SC_TYPE(paramType),
		SC_TYPE(sc_type_arc_pos_const_perm),
		relationAddr);

	if (!iter->next())
		return ScAddr();

	return iter->value(2);
}

ScAddr const & ScAgentAction::GetCommandInitiatedAddr()
{
	return msCommandInitiatedAddr;
}