/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_agent.hpp"

#include "../sc_debug.hpp"
#include "../sc_wait.hpp"

namespace
{
bool gInitializeResult = false;
bool gIsInitialized = false;

}  // namespace

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

ScAgent::~ScAgent() = default;

// ---------------------------
ScAgentAction::ScAgentAction(ScAddr const & cmdClassAddr, char const * name, sc_uint8 accessLvl)
  : ScAgent(name, accessLvl)
  , m_cmdClassAddr(cmdClassAddr)

{
}

ScAgentAction::~ScAgentAction() = default;

sc_result ScAgentAction::Run(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
{
  SC_UNUSED(otherAddr);

  ScAddr const cmdAddr = m_memoryCtx.GetEdgeTarget(edgeAddr);
  if (cmdAddr.IsValid())
  {
    if (m_memoryCtx.HelperCheckEdge(m_cmdClassAddr, cmdAddr, ScType::EdgeAccessConstPosPerm))
    {
      m_memoryCtx.EraseElement(edgeAddr);
      ScAddr progressAddr =
          m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosTemp, ScKeynodes::kCommandProgressedAddr, cmdAddr);
      assert(progressAddr.IsValid());
      ScAddr resultAddr = m_memoryCtx.CreateNode(ScType::NodeConstStruct);
      assert(resultAddr.IsValid());

      sc_result const resCode = RunImpl(cmdAddr, resultAddr);

      m_memoryCtx.EraseElement(progressAddr);

      ScAddr const commonEdge = m_memoryCtx.CreateEdge(ScType::EdgeDCommonConst, cmdAddr, resultAddr);

      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::kNrelResult, commonEdge);
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::GetResultCodeAddr(resCode), resultAddr);
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosTemp, ScKeynodes::kCommandFinishedAddr, cmdAddr);

      return SC_RESULT_OK;
    }
  }

  return SC_RESULT_ERROR;
}

ScAddr ScAgentAction::GetParam(ScAddr const & cmdAddr, ScAddr const & relationAddr, ScType const & paramType) const
{
  ScIterator5Ptr iter = m_memoryCtx.Iterator5(
      cmdAddr, ScType::EdgeAccessConstPosPerm, paramType, ScType::EdgeAccessConstPosPerm, relationAddr);

  if (!iter->Next())
    return {};

  return iter->Get(2);
}

ScAddr ScAgentAction::GetParam(ScAddr const & cmdAddr, size_t index) const
{
  return GetParam(cmdAddr, ScKeynodes::GetRrelIndex(index), ScType());
}

ScAddr const & ScAgentAction::GetCommandInitiatedAddr()
{
  return ScKeynodes::kCommandInitiatedAddr;
}

ScAddr const & ScAgentAction::GetCommandFinishedAddr()
{
  return ScKeynodes::kCommandFinishedAddr;
}

ScAddr const & ScAgentAction::GetNrelResultAddr()
{
  return ScKeynodes::kNrelResult;
}

ScAddr ScAgentAction::CreateCommand(ScMemoryContext & ctx, ScAddr const & cmdClassAddr, ScAddrVector const & params)
{
  if (params.size() >= ScKeynodes::GetRrelIndexNum())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "You should use <= " + std::to_string(ScKeynodes::GetRrelIndexNum()) + " params");

  SC_ASSERT(cmdClassAddr.IsValid(), ());

  ScAddr const cmdInstanceAddr = ctx.CreateNode(ScType::NodeConst);
  SC_ASSERT(cmdInstanceAddr.IsValid(), ());
  ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, cmdClassAddr, cmdInstanceAddr);

  for (size_t i = 0; i < params.size(); ++i)
  {
    ScAddr const edgeCommon = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, cmdInstanceAddr, params[i]);
    SC_ASSERT(edgeCommon.IsValid(), ());
    ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::GetRrelIndex(i), edgeCommon);
  }

  return cmdInstanceAddr;
}

bool ScAgentAction::InitiateCommand(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  // TODO: add blocks (locks) to prevent adding command to initiated set twicely

  // check if command is in progress
  if (ctx.HelperCheckEdge(ScKeynodes::kCommandProgressedAddr, cmdAddr, ScType::EdgeAccessConstPosTemp))
    return false;

  if (ctx.HelperCheckEdge(ScKeynodes::kCommandInitiatedAddr, cmdAddr, ScType::EdgeAccessConstPosTemp))
    return false;

  return ctx.CreateEdge(ScType::EdgeAccessConstPosTemp, ScKeynodes::kCommandInitiatedAddr, cmdAddr).IsValid();
}

bool ScAgentAction::InitiateCommandWait(
    ScMemoryContext & ctx,
    ScAddr const & cmdAddr,
    uint32_t waitTimeOutMS /* = 5000 */)
{
  ScWaitActionFinished waiter(ctx, cmdAddr);
  waiter.SetOnWaitStartDelegate([&]() {
    ScAgentAction::InitiateCommand(ctx, cmdAddr);
  });
  return waiter.Wait(waitTimeOutMS);
}

ScAddr ScAgentAction::GetCommandResultAddr(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  ScIterator5Ptr it = ctx.Iterator5(
      cmdAddr,
      ScType::EdgeDCommonConst,
      ScType::NodeConstStruct,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::kNrelResult);

  if (it->Next())
    return it->Get(2);

  return {};
}

sc_result ScAgentAction::GetCommandResultCode(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  ScAddr const addr = GetCommandResultCodeAddr(ctx, cmdAddr);
  return addr.IsValid() ? ScKeynodes::GetResultCodeByAddr(addr) : SC_RESULT_UNKNOWN;
}

ScAddr ScAgentAction::GetCommandResultCodeAddr(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  ScAddr const resultAddr = GetCommandResultAddr(ctx, cmdAddr);
  if (!resultAddr.IsValid())
    return {};

  ScTemplate templ;
  templ.Triple(ScKeynodes::kScResult, ScType::EdgeAccessVarPosPerm, ScType::NodeVarClass >> "result_class");
  templ.Triple("result_class", ScType::EdgeAccessVarPosPerm, resultAddr);

  ScTemplateSearchResult searchResult;
  if (!ctx.HelperSearchTemplate(templ, searchResult))
    return {};

  SC_ASSERT(searchResult.Size() == 1, ());
  return searchResult[0]["result_class"];
}

ScAgentAction::State ScAgentAction::GetCommandState(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  ScIterator3Ptr it = ctx.Iterator3(ScType::NodeConstClass, ScType::EdgeAccessConstPosTemp, cmdAddr);

  ScAddr stateAddr;
  while (it->Next())
  {
    if (ctx.HelperCheckEdge(ScKeynodes::kCommandStateAddr, it->Get(0), ScType::EdgeAccessConstPosPerm))
    {
      stateAddr = it->Get(0);
    }
  }

  if (stateAddr == ScKeynodes::kCommandFinishedAddr)
    return State::Finished;
  else if (stateAddr == ScKeynodes::kCommandInitiatedAddr)
    return State::Initiated;
  else if (stateAddr == ScKeynodes::kCommandProgressedAddr)
    return State::Progress;

  return State::Unknown;
}

bool ScAgentAction::IsCommandFishined(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  return GetCommandState(ctx, cmdAddr) == State::Finished;
}

bool ScAgentAction::IsCommandInitiated(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  return GetCommandState(ctx, cmdAddr) == State::Initiated;
}

bool ScAgentAction::IsCommandInProgress(ScMemoryContext & ctx, ScAddr const & cmdAddr)
{
  return GetCommandState(ctx, cmdAddr) == State::Progress;
}
