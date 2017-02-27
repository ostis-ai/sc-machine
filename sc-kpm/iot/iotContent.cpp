/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "iotKeynodes.hpp"
#include "iotUtils.hpp"

#include "sc-memory/cpp/sc_memory.hpp"

#include "iotContent.hpp"

namespace iot
{

SC_AGENT_ACTION_IMPLEMENTATION(AAddContentAgent)
{
  SC_ASSERT(requestAddr.IsValid(), ());

  // determine device class
  ScAddr const deviceAddr = GetParam(requestAddr, Keynodes::rrel_1, ScType::NodeConstMaterial);
  if (!deviceAddr.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  ScAddr const productClassAddr = GetParam(requestAddr, Keynodes::rrel_2, ScType::NodeConstClass);
  if (!productClassAddr.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;
  
  ScAddr const massLinkAddr = GetParam(requestAddr, Keynodes::rrel_3, ScType::Link);
  if (!massLinkAddr.IsValid())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // create content set if it doesn't exists
  ScIterator5Ptr iterContent = m_memoryCtx.Iterator5(
        ScType::NodeConstTuple,
        ScType::EdgeDCommonConst,
        deviceAddr,
        ScType::EdgeAccessConstPosPerm,
        Keynodes::nrel_content);

  ScAddr contentSet;
  if (!iterContent->Next())
  {
    contentSet = m_memoryCtx.CreateNode(ScType::NodeConstTuple);
    SC_ASSERT(contentSet.IsValid(), ());
    ScAddr const edgeCommon = m_memoryCtx.CreateEdge(ScType::EdgeDCommonConst, contentSet, deviceAddr);
    SC_ASSERT(edgeCommon.IsValid(), ());
    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::nrel_content, edgeCommon);
  }
  else
  {
    contentSet = iterContent->Get(0);
  }

  // create product instance
  ScAddr const product = m_memoryCtx.CreateNode(ScType::NodeConstMaterial);
  SC_ASSERT(product.IsValid(), ());
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, productClassAddr, product);

  utils::setMass(m_memoryCtx, product, massLinkAddr);

  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, contentSet, product);

  return SC_RESULT_OK;
}

// ------------------------------------
SC_AGENT_ACTION_IMPLEMENTATION(AGetContentAgent)
{
  ScIterator3Ptr iter = m_memoryCtx.Iterator3(
        requestAddr,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConstMaterial);

  if (!iter->Next())
    return SC_RESULT_ERROR_INVALID_PARAMS;

  ScAddr const deviceAddr = iter->Get(2);

  ScIterator5Ptr iter5 = m_memoryCtx.Iterator5(
        ScType::NodeConstTuple,
        ScType::EdgeDCommonConst,
        deviceAddr,
        ScType::EdgeAccessConstPosPerm,
        Keynodes::nrel_content);

  if (!iter5->Next())
    return SC_RESULT_ERROR;

  ScAddr const contentSet = iter5->Get(0);

  ScIterator3Ptr iterContent = m_memoryCtx.Iterator3(
        contentSet,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConstMaterial);

  while (iterContent->Next())
    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, resultAddr, iterContent->Get(2));

  return SC_RESULT_OK;
}

} // namespace iot
