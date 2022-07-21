/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_factory.hpp"

#define SC_SERVER_INFO "Info"
#define SC_SERVER_DEBUG "Debug"
#define SC_SERVER_ERROR "Error"

std::unique_ptr<ScServer> ScServerFactory::ConfigureScServer(
    const ScParams & serverParams,
    sc_memory_params memoryParams)
{
  std::unique_ptr<ScServer> server = std::unique_ptr<ScServer>(new ScServerImpl(
      serverParams.at("host"),
      std::stoi(serverParams.at("port")),
      serverParams.at("log_file"),
      std::stoi(serverParams.at("sync_actions")),
      memoryParams));

  std::string const & logLevel = serverParams.at("log_level");
  if (logLevel == SC_SERVER_INFO)
    server->SetMessageChannels(ScServerLogMessages::app);
  else if (logLevel == SC_SERVER_DEBUG)
    server->SetMessageChannels(ScServerLogMessages::all);
  else if (logLevel == SC_SERVER_ERROR)
    server->SetMessageChannels(ScServerLogMessages::fail);
  server->SetErrorChannels(ScServerLogErrors::all);

  return server;
}
