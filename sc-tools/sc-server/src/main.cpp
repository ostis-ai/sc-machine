/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <boost/program_options.hpp>

#include <iostream>
#include <atomic>
#include <thread>

#include <sc-memory/sc_debug.hpp>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/utils/sc_signal_handler.hpp>

#include "sc-config/sc_config.hpp"
#include "sc_memory_config.hpp"

#include "sc_server_factory.hpp"

sc_int main(sc_int argc, sc_char * argv[])
try
{
  boost::program_options::options_description options("Sc-server usage");
  options.add_options()("help", "Display this message")(
      "config,c", boost::program_options::value<std::string>(), "Path to configuration file")(
      "host,h", boost::program_options::value<std::string>(), "Sc-server host name, ip-address")(
      "port,p", boost::program_options::value<size_t>(), "Sc-server port")(
      "extensions_path,e", boost::program_options::value<std::string>(), "Path to directory with sc-memory extensions")(
      "repo_path,r", boost::program_options::value<std::string>(), "Path to kb.bin folder")(
      "verbose,v", "Flag to don't save sc-memory state on exit")("clear", "Flag to clear sc-memory on start");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options).run(), vm);
  boost::program_options::notify(vm);

  if (vm.count("help"))
  {
    std::cout << options;
    return EXIT_SUCCESS;
  }

  std::string configFile;
  if (vm.count("config"))
    configFile = vm["config"].as<std::string>();

  ScParams serverParams;
  std::vector<std::string> keys = {"host", "port"};
  for (auto const & key : keys)
  {
    if (vm.count(key))
      serverParams.insert({key, vm[key].as<std::string>()});
  }

  ScConfig config{configFile};
  auto serverConfig = config["sc-server"];
  for (auto const & key : *serverConfig)
    serverParams.insert({key, serverConfig[key]});

  ScParams memoryParams;
  keys = {"extensions_path", "repo_path", "verbose", "clear"};
  for (auto const & key : keys)
  {
    if (vm.count(key))
      memoryParams.insert({key, vm[key].as<std::string>()});
  }

  ScMemoryConfig memoryConfig{config, memoryParams};

  std::atomic_bool is_run = {true};
  utils::ScSignalHandler::Initialize();
  utils::ScSignalHandler::m_onTerminate = [&is_run]() {
    is_run = false;
  };

  ScServer * server = ScServerFactory::ConfigureScServer(serverParams, memoryConfig.GetParams());
  try
  {
    server->Run();
  }
  catch (std::exception const & e)
  {
    server->LogError(0, std::string(e.what()));
    server->Stop();
    delete server;

    return EXIT_FAILURE;
  }

  while (is_run)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  server->Stop();
  delete server;

  return EXIT_SUCCESS;
}
catch (utils::ScException const & ex)
{
  SC_LOG_ERROR(ex.Description());
}
