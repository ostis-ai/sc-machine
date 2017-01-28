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
  ScAddr tvAddr = GetParam(requestAddr, Keynodes::rrel_1, ScType::Unknown);
  ScAddr programAddr = GetParam(requestAddr, Keynodes::rrel_2, ScType::Unknown);

  if (!tvAddr.IsValid() || !programAddr.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // remove old program relation
  {
    ScIterator5Ptr iter = m_memoryCtx.Iterator5(
          tvAddr,
          ScType::EdgeDCommonConst,
          ScType::NodeConst,
          ScType::EdgeAccessConstPosPerm,
          Keynodes::nrel_tv_program);

    if (iter->Next())
      m_memoryCtx.EraseElement(iter->Get(2));
  }

  // create link to new program
  {
    ScAddr const edge = m_memoryCtx.CreateEdge(ScType::EdgeDCommonConst, tvAddr, programAddr);
    if (!edge.IsValid())
      return SC_RESULT_ERROR;

    ScAddr const edgeRel = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::nrel_tv_program, edge);
    if (!edgeRel.IsValid())
      return SC_RESULT_ERROR;
  }

  return SC_RESULT_OK;
}

} // namespace iot
