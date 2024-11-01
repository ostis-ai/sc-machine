/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_builder_runner.hpp"

#include <iostream>

#include <sc-config/sc_options.hpp>
#include <sc-config/sc_config.hpp>
#include <sc-config/sc_memory_config.hpp>

#include "sc-builder/builder.hpp"

void PrintStartMessage(std::string const & binaryName)
{
  std::cout
      << "Usage:\n"
      << "  " << binaryName << " -i <kb-directory or repo-path> -o <binary-path> [options]\n\n"
      << "Required options:\n"
      << "  --input|-i                              Provide a path to directory with knowledge base sources "
         "(.scs, .gwf) or to <repo-name>.path file.\n"
      << "                                          This file can contain knowledge base directories (or sources) "
         "that should/shouldn't be translated to binaries.\n"
      << "  --output|-o                             Provide a path to directory where the knowledge base "
         "binaries will be generated.\n\n"
      << "Options:\n"
      << "  --config|-c <config-name>.ini           Provide a path to configuration file.\n"
         "                                          Configuration file can be used to set additional (optional) "
         "options for "
      << binaryName << ".\n"
      << "  --clear                                 Run sc-builder in the mode when it overwrites "
         "existing knowledge base binaries.\n"
      << "  --version                               Display version of " << binaryName << ".\n"
      << "  --help                                  Display this help message.\n";
}

sc_int RunBuilder(sc_int argc, sc_char * argv[])
try
{
  std::string binaryName{argv[0]};

  ScOptions options{argc, argv};
  if (options.Has({"help"}))
  {
    PrintStartMessage(binaryName);
    return EXIT_SUCCESS;
  }

  if (options.Has({"version"}))
  {
    std::cout << ScMemoryConfig::GetVersion() << std::endl;
    return EXIT_SUCCESS;
  }

  BuilderParams params;
  if (options.Has({"input", "i"}))
    params.m_inputPath = options[{"input", "i"}].second;
  else
  {
    std::cout << "Error: Input path is required. Use --input|-i to specify the path to the knowledge base sources.\n";
    std::cout << "For more information, run with --help.\n";
    return EXIT_FAILURE;
  }

  if (options.Has({"output", "o"}))
    params.m_outputPath = options[{"output", "o"}].second;
  else
  {
    std::cout << "Error: Output path is required. Use --output|-o to specify where the knowledge base binaries will be "
                 "generated.\n";
    std::cout << "For more information, run with --help.\n";
    return EXIT_FAILURE;
  }

  std::string configPath;
  if (options.Has({"config", "c"}))
    configPath = options[{"config", "c"}].second;

  ScParams memoryParams{options, {{"verbose", "v"}, {"clear"}}};
  if (!params.m_outputPath.empty())
    memoryParams.Insert({"binaries", params.m_outputPath});

  ScConfig config{configPath, {"binaries", "log_file", "input"}, {"extensions"}};
  ScMemoryConfig memoryConfig{config, memoryParams};

  sc_memory_params formedMemoryParams = memoryConfig.GetParams();
  if (params.m_inputPath.empty())
    params.m_inputPath = config["sc-builder"]["input"];

  params.m_resultStructureUpload = formedMemoryParams.init_memory_generated_upload;
  if (formedMemoryParams.init_memory_generated_structure != nullptr)
    params.m_resultStructureSystemIdtf = formedMemoryParams.init_memory_generated_structure;

  formedMemoryParams.dump_memory = SC_FALSE;
  formedMemoryParams.dump_memory_statistics = SC_FALSE;
  formedMemoryParams.user_mode = SC_FALSE;

  Builder builder;
  return builder.Run(params, formedMemoryParams) ? EXIT_SUCCESS : EXIT_FAILURE;
}

catch (utils::ScException const & ex)
{
  std::cout << ex.Message() << std::endl;
  return EXIT_FAILURE;
}
