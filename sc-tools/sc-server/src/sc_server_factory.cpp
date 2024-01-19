/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_factory.hpp"
#include "sc-server-impl/sc_server_logger_impl.hpp"

std::shared_ptr<ScServer> ScServerFactory::ConfigureScServer(
    ScParams const & serverParams,
    sc_memory_params memoryParams)
{
  sc_bool parallelActions = SC_TRUE;
  if (serverParams.count("sync_actions"))
  {
    SC_LOG_WARNING(
        "Option `sync_actions` is deprecated in sc-machine 0.9.0. It will be removed in sc-machine 0.10.0. Use option "
        "`parallel_actions` instead of.");
    parallelActions = serverParams.at("sync_actions") == "0";
  }
  if (serverParams.count("parallel_actions"))
    parallelActions = serverParams.at("parallel_actions") == "true";

  std::unique_ptr<ScServer> server = std::unique_ptr<ScServer>(new ScServerImpl(
      serverParams.count("host") ? serverParams.at("host") : "127.0.0.1",
      serverParams.count("port") ? std::stoi(serverParams.at("port")) : 8090,
      parallelActions,
      memoryParams));

  return server;
}

ScServerLogger * ScServerFactory::ConfigureScServerLogger(
    std::shared_ptr<ScServer> const & server,
    ScParams const & serverParams)
{
  ScServerLogger * logger = new ScServerLoggerImpl(
      server, serverParams.at("log_type"), serverParams.at("log_file"), serverParams.at("log_level"));

  return logger;
}
