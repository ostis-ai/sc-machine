#pragma once

#include "sc_server_impl.hpp"

class ScServerFactory
{
public:
  static ScServer * ConfigureScServer(sc_memory_params params)
  {
    auto * server = new ScServerImpl(8090, "/home/nikita/ostis-apps/develop/cim-models-storage-module/ostis-web-platform/sc-machine/sc-server/src/sc-server.log", params);

    server->SetMessageChannels(
        ScServerLogMessages::connect | ScServerLogMessages::disconnect | ScServerLogMessages::app);
    server->SetErrorChannels(ScServerLogErrors::all);

    return server;
  }
};
