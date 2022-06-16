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