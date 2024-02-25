/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_factory.hpp"
#include "sc-server-impl/sc_server_logger_impl.hpp"

std::shared_ptr<ScServer> ScServerFactory::ConfigureScServer(ScParams const & serverParams)
{
  sc_bool parallelActions = SC_TRUE;
  if (serverParams.Has("parallel_actions"))
    parallelActions = serverParams.Get<std::string>("parallel_actions") == "true";
  std::unique_ptr<ScServer> server = std::unique_ptr<ScServer>(new ScServerImpl(
      serverParams.Get<std::string>("host", "127.0.0.1"), serverParams.Get("port", 8090), parallelActions));

  return server;
}

ScServerLogger * ScServerFactory::ConfigureScServerLogger(
    std::shared_ptr<ScServer> const & server,
    ScParams const & serverParams)
{
  ScServerLogger * logger = new ScServerLoggerImpl(
      server,
      serverParams.Get<std::string>("log_type", DEFAULT_LOG_TYPE),
      serverParams.Get<std::string>("log_file", DEFAULT_LOG_FILE),
      serverParams.Get<std::string>("log_level", DEFAULT_LOG_LEVEL));

  return logger;
}
