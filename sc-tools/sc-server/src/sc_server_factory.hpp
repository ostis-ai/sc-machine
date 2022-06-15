#pragma once

#include "sc-server-impl/sc_server_impl.hpp"

using ScServerParams = std::map<std::string, std::string>;

class ScServerFactory
{
public:
  static std::unique_ptr<ScServer> ConfigureScServer(ScServerParams serverParams, sc_memory_params memoryParams);
};
