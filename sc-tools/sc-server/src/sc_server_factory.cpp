#include "sc_server_factory.hpp"

std::unique_ptr<ScServer> ScServerFactory::ConfigureScServer(ScServerParams serverParams, sc_memory_params memoryParams)
{
  std::unique_ptr<ScServer> server = std::unique_ptr<ScServer>(new ScServerImpl(
          serverParams["host"],
          std::stoi(serverParams["port"]),
          serverParams["log_file"], memoryParams));

  server->SetMessageChannels(
      ScServerLogMessages::connect | ScServerLogMessages::disconnect | ScServerLogMessages::app);
  server->SetErrorChannels(ScServerLogErrors::all);

  return server;
}
