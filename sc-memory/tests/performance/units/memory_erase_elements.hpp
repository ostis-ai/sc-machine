/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#pragma once

#include "memory_test.hpp"

class TestEraseElements : public TestMemory
{
public:
  void Run()
  {
    m_ctx->EraseElement(m_addr);
  }

  void Setup(size_t objectsNum) override
  {
    m_addr = m_ctx->GenerateNode(ScType::NodeConst);

    ScAddr const trgAttr = m_ctx->GenerateNode(ScType::NodeConstRole);
    ScAddr const srcAttr = m_ctx->GenerateNode(ScType::NodeConstRole);

    for (size_t i = 0; i < objectsNum; ++i)
    {
      ScAddr const trg = m_ctx->GenerateNode(ScType::NodeClass);
      ScAddr const connectorTrg = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, m_addr, trg);
      m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, trgAttr, connectorTrg);

      ScAddr const src = m_ctx->GenerateNode(ScType::NodeClass);
      ScAddr const connectorSrc = m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, m_addr, src);
      m_ctx->GenerateConnector(ScType::EdgeAccessConstPosPerm, srcAttr, connectorSrc);
    }
  }

private:
  ScAddr m_addr;
};
