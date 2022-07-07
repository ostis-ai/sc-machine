/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_server_test.hpp"

#include "sc-memory/sc_link.hpp"

class TestCreateLink : public TestScServer
{
public:
  void Run(std::unique_ptr<ScClient> const & client)
  {
    std::string const payloadString = ScMemoryJsonConverter::From(
        0,
        "create_elements",
        ScMemoryJsonPayload::array({
            {
                {"el", "link"},
                {"type", sc_type_link | sc_type_const},
                {"content", "edge_end"},
            },
        }));
    client->Send(payloadString);
  }
};
