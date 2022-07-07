/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_server_test.hpp"
#include "../../sc_memory_json_converter.hpp"

class TestCreateNode : public TestScServer
{
public:
  void Run(std::unique_ptr<ScClient> const & client)
  {
    std::string const payloadString = ScMemoryJsonConverter::From(
        0,
        "create_elements",
        ScMemoryJsonPayload::array({
            {
                {"el", "node"},
                {"type", sc_type_node | sc_type_const},
            },
        }));
    client->Send(payloadString);
  }
};
