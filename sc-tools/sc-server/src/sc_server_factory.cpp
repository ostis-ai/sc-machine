/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_factory.hpp"

std::unique_ptr<ScServer> ScServerFactory::ConfigureScServer(
    const ScParams & serverParams,
    sc_memory_params memoryParams)
{
  std::unique_ptr<ScServer> server = std::unique_ptr<ScServer>(new ScServerImpl(
      serverParams.at("host"),
      std::stoi(serverParams.at("port")),
      serverParams.at("log_type"),
      serverParams.at("log_file"),
      serverParams.at("log_level"),
      std::stoi(serverParams.at("sync_actions")),
      memoryParams));

  return server;
}
