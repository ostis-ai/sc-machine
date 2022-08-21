/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <atomic>
#include <iostream>
#include <sc-memory/sc_debug.hpp>
#include <sc-memory/utils/sc_signal_handler.hpp>
#include <thread>

#include "sc-config/sc_config.hpp"
#include "sc_memory_config.hpp"
#include "sc_options.hpp"
#include "sc_server_factory.hpp"

sc_int main(sc_int argc, sc_char * argv[])
try
{
  ScOptions options{argc, argv};

  if (options.Has({"help"}))
  {
    std::cout << "SC-SERVER USAGE\n\n"
              << "--config|-c -- Path to configuration file\n"
              << "--host|-h -- Sc-server host name, ip-address\n"
              << "--port|-p -- Sc-server port\n"
              << "--extensions_path|-e -- Path to directory with sc-memory extensions\n"
              << "--repo_path|-r -- Path to kb.bin folder\n"
              << "--verbose|-v -- Flag to don't save sc-memory state on exit\n"
              << "--clear -- Flag to clear sc-memory on start\n"
              << "--help -- Display this message\n\n";
    return EXIT_SUCCESS;
  }

  std::string configFile;
  if (options.Has({"config", "c"}))
    configFile = options[{"config", "c"}].second;

  std::vector<std::vector<std::string>> keys = {{"host", "h"}, {"port", "p"}};
  ScParams serverParams{options, keys};

  std::vector<std::string> const pathKeys = {"repo_path", "extensions_path", "log_file"};
  ScConfig config{configFile, pathKeys};
  auto serverConfig = config["sc-server"];
  for (auto const & key : *serverConfig)
    serverParams.insert({key, serverConfig[key]});

  keys = {{"extensions_path", "e"}, {"repo_path", "r"}, {"verbose", "v"}, {"clear"}};
  ScParams memoryParams{options, keys};

  ScMemoryConfig memoryConfig{config, std::move(memoryParams)};

  auto server = ScServerFactory::ConfigureScServer(serverParams, memoryConfig.GetParams());
  try
  {
    server->Run();
  }
  catch (utils::ScException const & e)
  {
    SC_LOG_ERROR(e.Description());
    server->Stop();

    return EXIT_FAILURE;
  }
  catch (std::exception const & e)
  {
    SC_LOG_ERROR(e.what());
    server->Stop();

    return EXIT_FAILURE;
  }

  std::atomic_bool isRun = {SC_TRUE};
  utils::ScSignalHandler::Initialize();
  utils::ScSignalHandler::m_onTerminate = [&isRun]() {
    isRun = SC_FALSE;
  };

  while (isRun)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  server->Stop();

  return EXIT_SUCCESS;
}
catch (utils::ScException const & e)
{
  SC_LOG_ERROR(e.Description());
}
