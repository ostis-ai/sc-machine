/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_server_setup.hpp"

#include <iostream>
#include <atomic>
#include <thread>

#include <sc-memory/sc_debug.hpp>
#include <sc-memory/utils/sc_signal_handler.hpp>

#include "sc_server_factory.hpp"
#include "sc_server_module.hpp"

sc_bool RunServer(ScParams const & serverParams, std::shared_ptr<ScServer> & server)
{
  sc_bool status = SC_FALSE;
  try
  {
    server = ScServerFactory::ConfigureScServer(serverParams);
    ScServerLogger * logger = ScServerFactory::ConfigureScServerLogger(server, serverParams);
    server->Run();
    server->ResetLogger(logger);

    status = SC_TRUE;
  }
  catch (utils::ScException const & e)
  {
    server->ResetLogger();
    server->LogMessage(ScServerErrorLevel::error, e.Message());
  }
  // LCOV_EXCL_START
  catch (std::exception const & e)
  {
    server->ResetLogger();
    server->LogMessage(ScServerErrorLevel::error, e.what());
  }
  // LCOV_EXCL_STOP

  return status;
}

sc_bool StopServer(std::shared_ptr<ScServer> const & server)
{
  sc_bool status = SC_FALSE;
  try
  {
    server->ResetLogger();
    server->Stop();
    status = SC_TRUE;
  }
  // LCOV_EXCL_START
  catch (std::exception const & e)
  {
    server->LogMessage(ScServerErrorLevel::error, e.what());
  }
  // LCOV_EXCL_STOP
  return status;
}
