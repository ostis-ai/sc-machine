/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <boost/program_options.hpp>

#include <iostream>

#include <sc-memory/sc_debug.hpp>
#include <sc-memory/sc_memory.hpp>
#include <sc-memory/utils/sc_signal_handler.hpp>
#include "utils/parser.hpp"

#include <atomic>
#include <thread>
#include <map>

int main(int argc, char * argv[])
try
{
  boost::program_options::options_description options_description("Builder usage");
  options_description.add_options()
      ("help", "Display this message")
      ("extensions,e", boost::program_options::value<std::string>(), "Path to directory with sc-memory extensions")
      ("kb", boost::program_options::value<std::string>(), "Path to kb.bin folder")
      ("verbose,v", "Flag to don't save sc-memory state on exit")
      ("clear", "Flag to clear sc-memory on start")
      ("config,c", boost::program_options::value<std::string>(), "Path to configuration file");

  boost::program_options::variables_map vm;
  boost::program_options::store(
      boost::program_options::command_line_parser(argc, argv).options(options_description).run(), vm);
  boost::program_options::notify(vm);


  std::string configFile;
  if (vm.count("config"))
    configFile = vm["config"].as<std::string>();

  std::map<std::string, std::string> conf_file = parse_config(configFile);

  std::string extPath;
  if (vm.count("extensions"))
    extPath = vm["extensions"].as<std::string>();
  else
    extPath = conf_file["ext"];

  std::string repoPath;
  if (vm.count("kb"))
    repoPath = vm["kb"].as<std::string>();
  else
    repoPath = conf_file["path"];

  bool saveState = true;
  if (vm.count("verbose"))
    saveState = false;

  bool clear = false;
  if (vm.count("clear"))
    clear = true;

  if (vm.count("help"))
  {
    std::cout << options_description;
    return 0;
  }

  std::atomic_bool isRun = { true };
  utils::ScSignalHandler::Initialize();
  utils::ScSignalHandler::m_onTerminate = [&isRun]() {
    isRun = false;
  };

  sc_memory_params params;
  sc_memory_params_clear(&params);

  params.clear = clear ? SC_TRUE : SC_FALSE;
  params.config_file = configFile.c_str();
  params.enabled_exts = nullptr;
  params.ext_path = extPath.c_str();
  params.repo_path = repoPath.c_str();

  ScMemory::Initialize(params);

  while (isRun)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  ScMemory::Shutdown(saveState);

  return EXIT_SUCCESS;  // : EXIT_FAILURE;
}
catch (utils::ScException const & ex)
{
  SC_LOG_ERROR(ex.Description());
}
