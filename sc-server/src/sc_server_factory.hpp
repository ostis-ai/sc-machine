#pragma once

#include "src/sc-server-impl/sc_server_impl.hpp"

using ScServerParams = std::map<std::string, std::string>;

class ScServerFactory
{
public:
  static ScServer * ConfigureScServer(ScServerParams & serverParams)
  {
    auto * server = new ScServerImpl("localhost", 8090, "", GetScMemoryParams(serverParams));

    server->SetMessageChannels(
        ScServerLogMessages::connect | ScServerLogMessages::disconnect | ScServerLogMessages::app);
    server->SetErrorChannels(ScServerLogErrors::all);

    return server;
  }

  static sc_memory_params GetScMemoryParams(ScServerParams & serverParams)
  {
    sc_memory_params memoryParams;

    memoryParams.ext_path = serverParams["extensions_path"].c_str();
    memoryParams.repo_path = serverParams["repo_path"].c_str();
    memoryParams.max_threads = std::stoi(serverParams["max_threads"]);
    memoryParams.max_loaded_segments = std::stoi(serverParams["max_loaded_segments"]);
    memoryParams.enabled_exts = nullptr;
    memoryParams.clear = serverParams.count("clear") ? SC_TRUE : SC_FALSE;

    return memoryParams;
  }
};
