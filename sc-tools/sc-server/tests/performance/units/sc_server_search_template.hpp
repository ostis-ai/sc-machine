/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_server_test.hpp"
#include "../../sc_memory_json_converter.hpp"

class TestSearchTemplate : public TestScServer
{
public:
  void Run(std::unique_ptr<ScClient> const & client)
  {
    std::string const payloadString = ScMemoryJsonConverter::From(
        0,
        "search_template",
        ScMemoryJsonPayload::array({
            {
                {
                    {"type", "addr"},
                    {"value", m_nodes[random() % m_nodes.size()].Hash()},
                    {"alias", "_src"},
                },
                {
                    {"type", "type"},
                    {"value", sc_type_arc_pos_var_perm | sc_type_var},
                    {"alias", "_edge1"},
                },
                {
                    {"type", "type"},
                    {"value", sc_type_node | sc_type_var},
                    {"alias", "_trg"},

                },
            },
        }));
    client->Send(payloadString);
  }

  void Setup(size_t edgeNum) override
  {
    size_t nodeNum = 10;
    m_nodes.reserve(nodeNum);
    for (size_t i = 0; i < nodeNum; ++i)
      m_nodes.push_back(m_ctx->CreateNode(ScType::NodeConst));

    for (size_t i = 0; i < edgeNum; ++i)
      m_ctx->CreateEdge(
          ScType::EdgeAccessConstPosPerm, m_nodes[random() % m_nodes.size()], m_nodes[random() % m_nodes.size()]);
  }

private:
  ScAddrVector m_nodes;
};
