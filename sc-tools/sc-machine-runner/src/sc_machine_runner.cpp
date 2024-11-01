/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-machine-runner/sc_machine_runner.hpp"

#include <iostream>
#include <atomic>
#include <thread>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_debug.hpp>
#include <sc-memory/utils/sc_signal_handler.hpp>

#include <sc-config/sc_options.hpp>
#include <sc-config/sc_config.hpp>
#include <sc-config/sc_memory_config.hpp>

void PrintStartMessage(std::string const & binaryName)
{
  std::cout
      << "Usage:\n"
      << "  " << binaryName << " [options]\n\n"
      << "Options:\n"
      << "  --config|-c <project>.ini                          Path to configuration file\n"
      << "  --kb-binaries|-b <directory>                       Path to directory with knowledge base binaries\n"
      << "  --extensions|-e <directory_1>;<directory_2>;...    Path to directories containing extensions\n"
      << "  --clear                                            Flag to clear sc-memory state on initialization\n"
      << "  --verbose|-v                                       Flag to prevent saving sc-memory state on shutdown\n"
      << "  --test|-t                                          Flag to test " << binaryName
      << " (if this flag is specified, sc-machine will start and stop immediately)\n"
      << "  --help                                             Display this help message\n\n";
}

sc_int RunMachine(sc_int argc, sc_char * argv[])
try
{
  std::string binaryName{argv[0]};

  ScOptions options{argc, argv};
  if (options.Has({"help"}))
  {
    PrintStartMessage(binaryName);
    return EXIT_SUCCESS;
  }

  std::string configPath;
  if (options.Has({"config", "c"}))
    configPath = options[{"config", "c"}].second;
  else
  {
    std::cout
        << "Error: Configuration file is required. Use --config|-c to specify the path to the configuration file.\n";
    std::cout << "For more information, run with --help.\n";
    return EXIT_FAILURE;
  }

  sc_bool saveOnShutdown = !options.Has({"verbose", "v"});

  ScMemory::ms_configPath = configPath;

  ScConfig config{configPath, {"extensions", "kb_binaries", "log_file"}};
  ScParams memoryParams{options, {{"extensions", "e"}, {"kb-binaries", "b"}, {"clear"}}};
  ScMemoryConfig memoryConfig{config, memoryParams};

  std::atomic_bool isRun;
  if (!ScMemory::Initialize(memoryConfig.GetParams()))
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
