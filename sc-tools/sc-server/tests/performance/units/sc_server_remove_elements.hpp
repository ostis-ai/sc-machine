/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_server_test.hpp"

class TestRemoveElements : public TestScServer
{
public:
  void Run(std::unique_ptr<ScClient> const & client)
  {
    std::string const payloadString =
        ScMemoryJsonConverter::From(0, "delete_elements", ScMemoryJsonPayload::array({m_addr.Hash()}));
    client->Send(payloadString);
  }

  void Setup(size_t objectsNum) override
  {
    m_addr = m_ctx->CreateNode(ScType::NodeConst);

    ScAddr const trgAttr = m_ctx->CreateNode(ScType::NodeConstRole);
    ScAddr const srcAttr = m_ctx->CreateNode(ScType::NodeConstRole);

    for (size_t i = 0; i < objectsNum; ++i)
    {
      ScAddr const trg = m_ctx->CreateNode(ScType::NodeClass);
      ScAddr const edgeTrg = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_addr, trg);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, trgAttr, edgeTrg);

      ScAddr const src = m_ctx->CreateNode(ScType::NodeClass);
      ScAddr const edgeSrc = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_addr, src);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, srcAttr, edgeSrc);
    }
  }

private:
  ScAddr m_addr;
};
