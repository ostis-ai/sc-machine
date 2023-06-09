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
      serverParams.count("host") ? serverParams.at("host") : "127.0.0.1",
      serverParams.count("port") ? std::stoi(serverParams.at("port")) : 8090,
      serverParams.at("log_type"),
      serverParams.at("log_file"),
      serverParams.at("log_level"),
      serverParams.count("sync_actions") ? std::stoi(serverParams.at("sync_actions")) : SC_FALSE,
      memoryParams));

  return server;
}
