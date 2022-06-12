#pragma once

#include "src/sc-server-impl/sc_server_impl.hpp"

using ScServerParams = std::map<std::string, std::string>;

class ScServerFactory
{
public:
  static ScServer * ConfigureScServer(ScServerParams & serverParams, sc_memory_params memoryParams)
  {
    auto * server =
        new ScServerImpl(serverParams["host"], std::stoi(serverParams["port"]), serverParams["log_file"], memoryParams);

    server->SetMessageChannels(
        ScServerLogMessages::connect | ScServerLogMessages::disconnect | ScServerLogMessages::app);
    server->SetErrorChannels(ScServerLogErrors::all);

    return server;
  }
};
