#pragma once

#include "sc-server-impl/sc_server_impl.hpp"
#include "sc_memory_config.hpp"

class ScServerFactory
{
public:
  static std::unique_ptr<ScServer> ConfigureScServer(const ScParams & serverParams, sc_memory_params memoryParams);
};
