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
#include "sc-server-module/sc_server_module.hpp"

void PrintStartMessage()
{
  std::cout << "SC-SERVER USAGE\n\n"
            << "--config|-c -- Path to configuration file\n"
            << "--host|-h -- Sc-server host name, ip-address\n"
            << "--port|-p -- Sc-server port\n"
            << "--extensions_path|-e -- Path to directory with sc-memory extensions\n"
            << "--repo_path|-r -- Path to kb.bin folder\n"
            << "--clear -- Flag to clear sc-memory state on initialize\n"
            << "--verbose|-v -- Flag to don't save sc-memory state on shutdown\n"
            << "--test|-t -- Flag to test sc-server (sc-server with this option runs and stops)\n"
            << "--help -- Display this message\n\n";
}

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

sc_int BuildAndRunServer(sc_int argc, sc_char * argv[])
try
{
  ScOptions options{argc, argv};

  if (options.Has({"help"}))
  {
    PrintStartMessage();
    return EXIT_SUCCESS;
  }

  SC_LOG_WARNING(
      "Now sc-server is an extension and all extensions are loaded by executable `sc-machine`. The executable "
      "`sc-server` is deprecated in sc-machine 0.10.0. It will be removed in sc-machine 0.11.0. Use "
      "executable `sc-machine` instead.");

  std::string configFile;
  if (options.Has({"config", "c"}))
    configFile = options[{"config", "c"}].second;

  sc_bool saveOnShutdown = !options.Has({"verbose", "v"});

  ScMemory::ms_configPath = configFile;

  ScConfig config{configFile, {"repo_path", "extensions_path", "log_file"}};
  ScParams memoryParams{options, {{"extensions_path", "e"}, {"repo_path", "r"}, {"clear"}}};
  ScMemoryConfig memoryConfig{config, memoryParams};

  SC_LOG_WARNING(
      "Use the common configuration file `<config-name>.ini` to set port and host of sc-server. `Options `--host` and "
      "`--port` are deprecated as well.");
  ScServerModule::ms_serverParams = ScParams{options, {{"host", "h"}, {"port", "p"}}};

  std::atomic_bool isRun;
  if (ScMemory::Initialize(memoryConfig.GetParams()) == SC_FALSE)
    goto error;

  utils::ScSignalHandler::Initialize();

  isRun = !options.Has({"test", "t"});
  // LCOV_EXCL_START
  utils::ScSignalHandler::m_onTerminate = [&isRun]()
  {
    isRun = SC_FALSE;
  };

  while (isRun)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  // LCOV_EXCL_STOP

error:
  return ScMemory::Shutdown(saveOnShutdown) ? EXIT_SUCCESS : EXIT_FAILURE;
}

// LCOV_EXCL_START
catch (utils::ScException const & e)
{
  SC_LOG_ERROR(e.Description());
  return EXIT_FAILURE;
}

// LCOV_EXCL_STOP
