/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_machine_setup.hpp"

#include <iostream>
#include <atomic>
#include <thread>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_debug.hpp>
#include <sc-memory/utils/sc_signal_handler.hpp>

#include <sc-config/sc_options.hpp>
#include <sc-config/sc_memory_config.hpp>

void PrintStartMessage()
{
  std::cout << "SC-MACHINE USAGE\n\n"
            << "--config|-c -- Path to configuration .ini file\n"
            << "--extensions_path|-e -- Path to directory with sc-memory extensions\n"
            << "--repo_path|-r -- Path to kb.bin directory\n"
            << "--clear -- Flag to clear sc-memory state on initialize\n"
            << "--verbose|-v -- Flag to don't save sc-memory state on shutdown\n"
            << "--test|-t -- Flag to test sc-machine (sc-machine with this option runs and stops)\n"
            << "--help -- Display this message\n\n";
}

sc_int BuildAndRunMachine(sc_int argc, sc_char * argv[])
try
{
  ScOptions options{argc, argv};

  if (options.Has({"help"}))
  {
    PrintStartMessage();
    return EXIT_SUCCESS;
  }

  std::string configFile;
  if (options.Has({"config", "c"}))
    configFile = options[{"config", "c"}].second;

  sc_bool saveOnShutdown = !options.Has({"verbose", "v"});

  ScMemory::ms_configPath = configFile;

  ScConfig config{configFile, {"repo_path", "extensions_path", "log_file"}};
  ScParams memoryParams{options, {{"extensions_path", "e"}, {"repo_path", "r"}, {"clear"}}};
  ScMemoryConfig memoryConfig{config, memoryParams};

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
