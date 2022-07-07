/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <nlohmann/json.hpp>

#include "sc_client_defines.hpp"

using ScMemoryJsonPayload = nlohmann::json;

class ScMemoryJsonConverter
{
public:
  static std::string From(size_t messageId, std::string type, ScMemoryJsonPayload const & payload)
  {
    ScMemoryJsonPayload output;
    output["id"] = messageId;
    output["type"] = type;
    output["payload"] = payload;

    return output.dump();
  }
};
