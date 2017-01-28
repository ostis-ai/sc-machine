/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_agent.hpp"

#include "../sc_debug.hpp"

namespace
{

bool gInitializeResult = false;
bool gIsInitialized = false;

bool ResolveKeynodeImpl(ScMemoryContext & ctx, char const * str, ScAddr & outAddr)
{
  SC_ASSERT(ctx.IsValid(), ());

  return ctx.HelperResolveSystemIdtf(str, outAddr, true);
}

} // namespace

bool ScAgentInit(bool force /* = false */)
{
  if (force || !gIsInitialized)
  {
    gInitializeResult = ScAgentAction::InitGlobal();

    gIsInitialized = true;
  }

  return gInitializeResult;
}


ScAgent::ScAgent(char const * name, sc_uint8 accessLvl)
  : m_memoryCtx(accessLvl, name)
{
}

ScAgent::~ScAgent()
{
}

sc_result ScAgent::Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
{
  return SC_RESULT_ERROR;
}


// ---------------------------
ScAddr ScAgentAction::ms_commandInitiatedAddr;
ScAddr ScAgentAction::ms_commandProgressdAddr;
ScAddr ScAgentAction::ms_commandFinishedAddr;
ScAddr ScAgentAction::ms_nrelResult;

ScAddr ScAgentAction::ms_keynodeScResultOk;
ScAddr ScAgentAction::ms_keynodeScResultNo;
ScAddr ScAgentAction::ms_keynodeScResultError;
ScAddr ScAgentAction::ms_keynodeScResultErrorInvalidParams;
ScAddr ScAgentAction::ms_keynodeScResultErrorInvalidType;
ScAddr ScAgentAction::ms_keynodeScResultErrorIO;
ScAddr ScAgentAction::ms_keynodeScResultInvalidState;
ScAddr ScAgentAction::ms_keynodeScResultErrorNotFound;
ScAddr ScAgentAction::ms_keynodeScResultErrorNoWriteRights;
ScAddr ScAgentAction::ms_keynodeScResultErrorNoReadRights;

ScAgentAction::ScAgentAction(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl)
  : ScAgent(name, accessLvl)
  , m_cmdClassAddr(cmdClassAddr)

{
}

ScAgentAction::~ScAgentAction()
{
}

sc_result ScAgentAction::Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
{
  SC_UNUSED(otherAddr);

  ScAddr const cmdAddr = m_memoryCtx.GetEdgeTarget(edgeAddr);
  if (cmdAddr.IsValid())
  {
    if (m_memoryCtx.HelperCheckEdge(m_cmdClassAddr, cmdAddr, ScType::EdgeAccessConstPosPerm))
    {
      m_memoryCtx.EraseElement(edgeAddr);
      ScAddr progressAddr = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, ms_commandProgressdAddr, cmdAddr);
      assert(progressAddr.IsValid());
      ScAddr resultAddr = m_memoryCtx.CreateNode(ScType::NodeConstStruct);
      assert(resultAddr.IsValid());

      sc_result const resCode = RunImpl(cmdAddr, resultAddr);

      m_memoryCtx.EraseElement(progressAddr);

      ScAddr const commonEdge = m_memoryCtx.CreateEdge(ScType::EdgeDCommonConst, cmdAddr, resultAddr);
      SC_ASSERT(commonEdge.IsValid(), ());
      ScAddr const edge = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, ms_nrelResult, commonEdge);
      SC_ASSERT(edge.IsValid(), ());

      ScAddr const edgeResult = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, GetResultCodeAddr(resCode), resultAddr);
      SC_ASSERT(edgeResult.IsValid(), ());

      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, ms_commandFinishedAddr, cmdAddr);

      return SC_RESULT_OK;
    }
  }

  return SC_RESULT_ERROR;
}

ScAddr ScAgentAction::GetParam(ScAddr const & cmdAddr, ScAddr const & relationAddr, ScType const & paramType)
{
  ScIterator5Ptr iter = m_memoryCtx.Iterator5(cmdAddr,
                                              ScType::EdgeAccessConstPosPerm,
                                              paramType,
                                              ScType::EdgeAccessConstPosPerm,
                                              relationAddr);

  if (!iter->Next())
    return ScAddr();

  return iter->Get(2);
}

ScAddr const & ScAgentAction::GetCommandInitiatedAddr()
{
  return ms_commandInitiatedAddr;
}

ScAddr const & ScAgentAction::GetCommandFinishedAddr()
{
  return ms_commandFinishedAddr;
}

ScAddr const & ScAgentAction::GetNrelResultAddr()
{
  return ms_nrelResult;
}

ScAddr const & ScAgentAction::GetResultCodeAddr(sc_result resCode)
{
  switch (resCode)
  {
  case SC_RESULT_NO:
    return ms_keynodeScResultNo;
  case SC_RESULT_ERROR:
    return ms_keynodeScResultError;
  case SC_RESULT_OK:
    return ms_keynodeScResultOk;
  case SC_RESULT_ERROR_INVALID_PARAMS:
    return ms_keynodeScResultErrorInvalidParams;
  case SC_RESULT_ERROR_INVALID_TYPE:
    return ms_keynodeScResultErrorInvalidType;
  case SC_RESULT_ERROR_IO:
    return ms_keynodeScResultErrorIO;
  case SC_RESULT_ERROR_INVALID_STATE:
    return ms_keynodeScResultInvalidState;
  case SC_RESULT_ERROR_NOT_FOUND:
    return ms_keynodeScResultErrorNotFound;
  case SC_RESULT_ERROR_NO_WRITE_RIGHTS:
    return ms_keynodeScResultErrorNoWriteRights;
  case SC_RESULT_ERROR_NO_READ_RIGHTS:
    return ms_keynodeScResultErrorNoReadRights;
  };

  return ms_keynodeScResultError;
}