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

  std::vector<std::string> keys = {"host", "port"};
  ScParams serverParams{vm, keys};

  ScConfig config{configFile};
  auto serverConfig = config["sc-server"];
  for (auto const & key : *serverConfig)
    serverParams.insert({key, serverConfig[key]});

  keys = {"extensions_path", "repo_path", "verbose", "clear"};
  ScParams memoryParams{vm, keys};

  ScMemoryConfig memoryConfig{config, memoryParams};

  std::atomic_bool isRun = {SC_TRUE};
  utils::ScSignalHandler::Initialize();
  utils::ScSignalHandler::m_onTerminate = [&isRun]() {
    isRun = SC_FALSE;
  };

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
