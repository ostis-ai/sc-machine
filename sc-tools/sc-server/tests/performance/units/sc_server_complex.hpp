/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_server_test.hpp"
#include "../../sc_memory_json_converter.hpp"

class TestScServerComplex : public TestScServer
{
public:
  void Run(std::unique_ptr<ScClient> const & client)
  {
    std::string const payloadString =
        ScMemoryJsonConverter::From(0, "create_elements", m_payloads[random() % m_payloads.size()]);
    client->Send(payloadString);
  }

  void Setup(size_t elementsNum) override
  {
    m_nodes.reserve(elementsNum);
    for (size_t i = 0; i < elementsNum; ++i)
      m_nodes.push_back(m_ctx->CreateNode(ScType::NodeConst));

    m_payloads = {
        ScMemoryJsonPayload::array({
            {
                {"el", "node"},
                {"type", sc_type_node | sc_type_const},
            },
        }),
        ScMemoryJsonPayload::array({
            {
                {"el", "edge"},
                {"src",
                 {
                     {"type", "addr"},
                     {"value", m_nodes[random() % m_nodes.size()].Hash()},
                 }},
                {"trg",
                 {
                     {"type", "addr"},
                     {"value", m_nodes[random() % m_nodes.size()].Hash()},
                 }},
                {"type", sc_type_arc_pos_const_perm},
            },
        }),
        ScMemoryJsonPayload::array({
            {
                {"el", "link"},
                {"type", sc_type_link | sc_type_const},
                {"content", "edge_end"},
            },
        })};
  }

private:
  ScAddrVector m_nodes;

  std::vector<ScMemoryJsonPayload> m_payloads;
};
