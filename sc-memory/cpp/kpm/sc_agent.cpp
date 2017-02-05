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

size_t const kKeynodeRrelListNum = 10;
std::array<ScAddr, kKeynodeRrelListNum> kKeynodeRrelList;

} // namespace

bool ScAgentInit(bool force /* = false */)
{
  if (force || !gIsInitialized)
  {
    gInitializeResult = ScAgentAction::InitGlobal();

    // resolve rrel_n relations
    ScMemoryContext ctx(sc_access_lvl_make_min, "ScAgentInit");
    for (size_t i = 0; i < kKeynodeRrelListNum; ++i)
    {
      ScAddr & item = kKeynodeRrelList[i];
      if (!ctx.HelperResolveSystemIdtf("rrel_" + std::to_string(i + 1), item, ScType::NodeConstRole))
        gInitializeResult = false;
      SC_ASSERT(item.IsValid(), ());
    }

    // should be call after ScAgentAction::InitGlobal()
    gInitializeResult = gInitializeResult && ScAgentAction::InitSets(ctx);

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
ScAddr ScAgentAction::ms_nrelCommonTemplate;

ScAddr ScAgentAction::ms_keynodeScResult;
ScAddr ScAgentAction::ms_keynodeScResultUnknown;
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

bool ScAgentAction::InitSets(ScMemoryContext & ctx)
{
  bool result = true;

  // init sc_result set
  for (size_t i = 0; i < SC_RESULT_COUNT; ++i)
  {
    ScAddr const resAddr = GetResultCodeAddr(static_cast<sc_result>(i));
    result = result && resAddr.IsValid();
    if (!ctx.HelperCheckEdge(ms_keynodeScResult, resAddr, ScType::EdgeAccessConstPosPerm))
    {
      result = result && ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, ms_keynodeScResult, resAddr).IsValid();
    }
  }

  return result;
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

ScAddr ScAgentAction::GetParam(ScAddr const & cmdAddr, ScAddr const & relationAddr, ScType const & paramType) const
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

ScAddr ScAgentAction::GetParam(ScAddr const & cmdAddr, size_t index) const
{
  if (index >= kKeynodeRrelListNum)
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams,
      "You should use index in range[0; " + std::to_string(kKeynodeRrelListNum) + "]");
  }

  return GetParam(cmdAddr, kKeynodeRrelList[index], ScType());
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
  case SC_RESULT_UNKNOWN:
    return ms_keynodeScResultUnknown;
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

sc_result ScAgentAction::GetResultCodeByAddr(ScAddr const & resultClassAddr)
{
  if (!resultClassAddr.IsValid())
    return SC_RESULT_UNKNOWN;

  if (resultClassAddr == ms_keynodeScResultNo)
    return SC_RESULT_NO;
  else if (resultClassAddr == ms_keynodeScResultError)
    return SC_RESULT_ERROR;
  else if (resultClassAddr == ms_keynodeScResultOk)
    return SC_RESULT_OK;
  else if (resultClassAddr == ms_keynodeScResultErrorInvalidParams)
    return SC_RESULT_ERROR_INVALID_PARAMS;
  else if (resultClassAddr == ms_keynodeScResultErrorInvalidType)
    return SC_RESULT_ERROR_INVALID_TYPE;
  else if (resultClassAddr == ms_keynodeScResultErrorIO)
    return SC_RESULT_ERROR_IO;
  else if (resultClassAddr == ms_keynodeScResultInvalidState)
    return SC_RESULT_ERROR_INVALID_STATE;
  else if (resultClassAddr == ms_keynodeScResultErrorNotFound)
    return SC_RESULT_ERROR_NOT_FOUND;
  else if (resultClassAddr == ms_keynodeScResultErrorNoWriteRights)
    return SC_RESULT_ERROR_NO_WRITE_RIGHTS;
  else if (resultClassAddr == ms_keynodeScResultErrorNoReadRights)
    return SC_RESULT_ERROR_NO_READ_RIGHTS;
  
  return SC_RESULT_UNKNOWN;
}

ScAddr ScAgentAction::CreateCommand(ScMemoryContext & ctx, ScAddr const & cmdClassAddr, ScAddrVector const & params)
{
  if (params.size() >= kKeynodeRrelListNum)
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You should use <= " + std::to_string(kKeynodeRrelListNum) + " params");

  SC_ASSERT(cmdClassAddr.IsValid(), ());

  ScAddr const cmdInstanceAddr = ctx.CreateNode(ScType::NodeConst);
  SC_ASSERT(cmdInstanceAddr.IsValid(), ());
  ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, cmdClassAddr, cmdInstanceAddr);
  SC_ASSERT(edge.IsValid(), ());
  
  for (size_t i = 0; i < params.size(); ++i)
  {
    ScAddr const edgeCommon = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, cmdInstanceAddr, params[i]);
    SC_ASSERT(edgeCommon.IsValid(), ());
    ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, kKeynodeRrelList[i], edgeCommon);
  }

  return cmdInstanceAddr;
}

bool ScAgentAction::InitiateCommand(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  // TODO: add blocks (locks) to prevent adding command to initiated set twicely

  // check if command is in progress
  if (ctx.HelperCheckEdge(ms_commandProgressdAddr, cmdAddr, ScType::EdgeAccessConstPosPerm))
    return false;

  if (ctx.HelperCheckEdge(ms_commandInitiatedAddr, cmdAddr, ScType::EdgeAccessConstPosPerm))
    return false;

  return ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, ms_commandInitiatedAddr, cmdAddr).IsValid();
}

ScAddr ScAgentAction::GetCommandResultAddr(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  ScIterator5Ptr it = ctx.Iterator5(
    cmdAddr,
    ScType::EdgeDCommonConst,
    ScType::NodeConstStruct,
    ScType::EdgeAccessConstPosPerm,
    ms_nrelResult);

  if (it->Next())
    return it->Get(2);

  return ScAddr();
}

sc_result ScAgentAction::GetCommandResultCode(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  ScAddr const resultAddr = GetCommandResultAddr(ctx, cmdAddr);
  if (!resultAddr.IsValid())
    return SC_RESULT_UNKNOWN;

  ScTemplate templ;
  templ.Triple(
    ms_keynodeScResult,
    ScType::EdgeAccessVarPosPerm,
    ScType::NodeVarClass >> "result_class");
  templ.Triple(
    "result_class",
    ScType::EdgeAccessVarPosPerm,
    resultAddr);

  ScTemplateSearchResult searchResult;
  if (!ctx.HelperSearchTemplate(templ, searchResult))
    return SC_RESULT_UNKNOWN;

  //ScIterator3Ptr it1 = ctx.Iterator3(
  //  ms_keynodeScResult,
  //  ScType::EdgeAccessConstPosPerm,
  //  ScType());
  //while (it1->Next())
  //{
  //  ScIterator3Ptr it2 = ctx.Iterator3(
  //    it1->Get(2),
  //    ScType::EdgeAccessConstPosPerm,
  //    resultAddr);
  //  if (it2->Next())
  //    return GetResultCodeByAddr(it2->Get(2));
  //}

  SC_ASSERT(searchResult.Size() == 1, ());
  return GetResultCodeByAddr(searchResult[0][2]);
  //return SC_RESULT_UNKNOWN;
}