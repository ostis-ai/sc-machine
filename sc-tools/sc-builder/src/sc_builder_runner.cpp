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

void PrintHelpMessage(std::string const & binaryName)
{
  std::cout << "Usage:\n"
            << "  " << binaryName << " -i <kb-directory or repo-path> -o <binary-path> [options]\n\n"
            << "Required options:\n"
            << "  --input|-i                              Provide a path to directory with knowledge base sources "
               "(.scs, .gwf) or to <repo-name>.path file.\n"
            << "                                          This file contains knowledge base directories (or sources) "
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
    PrintHelpMessage(binaryName);
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

  if (options.Has({"output", "o"}))
    params.m_outputPath = options[{"output", "o"}].second;

  std::string configPath;
  if (options.Has({"config", "c"}))
    configPath = options[{"config", "c"}].second;

  ScParams memoryParams{options, {{"verbose", "v"}, {"clear"}}};
  if (!params.m_outputPath.empty())
    memoryParams.Insert({"storage", params.m_outputPath});

  ScConfig config{configPath, {"storage", "log_file", "input_path", "input", "output"}, {"extensions"}};
  ScMemoryConfig memoryConfig{config, memoryParams};
  sc_memory_params formedMemoryParams = memoryConfig.GetParams();

  if (!config.IsValid() && params.m_inputPath.empty())
  {
    std::cout << "Error: Input path is required, but it is missing.\n"
              << "You should specify the path to the knowledge base sources using the --input|-i option or "
              << "in the configuration file (<config>.ini) under the `input` option in the [sc-builder] group.\n"
              << "Please ensure that this path points to a valid directory containing .scs or .gwf files.\n"
              << "If this path is specified in the configuration file, make sure the path to this configuration file "
                 "is correct and accessible.\n"
              << "For more information, run with --help.\n";
    return EXIT_FAILURE;
  }

  if (!config.IsValid() && params.m_outputPath.empty())
  {
    std::cout
        << "Error: Output path is required, but it is missing.\n"
        << "You should specify where the knowledge base binaries will be generated using the --output|-o option or "
        << "in the configuration file (<config>.ini) under the `output` option in the [sc-builder] group.\n"
        << "Ensure that this path is a valid directory where you have write permissions and that it exists.\n"
        << "If this path is specified in the configuration file, please verify that the path to this configuration "
           "file is correct and accessible.\n"
        << "For more information, run with --help.\n";
    return EXIT_FAILURE;
  }

  ScConfigGroup const & builderGroup = config["sc-builder"];
  if (params.m_inputPath.empty())
  {
    std::string const & inputPath = builderGroup["input_path"];
    if (!inputPath.empty())
    {
      SC_LOG_WARNING(
          "Option `input_path` in `[sc-builder]` group is deprecated since sc-machine 0.10.0. Use option "
          "`repo_path` instead.");
      params.m_inputPath = inputPath;
    }
  }

  if (params.m_inputPath.empty())
    params.m_inputPath = builderGroup["input"];

  if (params.m_inputPath.empty())
  {
    std::cout
        << "Error: Input path is required, but it is missing.\n"
        << "Please, specify the path to the knowledge base sources using the --input|-i option on the command line, "
        << "or include this path in the configuration file (<config>.ini) under the `input` option in the [sc-builder] "
           "group.\n"
        << "Ensure that this path points to a valid directory containing .scs or .gwf files.\n"
        << "For more information, run with --help.\n";
    return EXIT_FAILURE;
  }

  if (params.m_outputPath.empty())
    params.m_outputPath = builderGroup["output"];

  if (params.m_outputPath.empty())
    params.m_outputPath = formedMemoryParams.storage;

  if (params.m_outputPath.empty())
  {
    std::cout << "Error: Output path is required, but it is missing.\n"
              << "Please, specify where the knowledge base binaries will be generated using the --output|-o option on "
                 "the command line, "
              << "or include this path in the configuration file (<config>.ini) under the `output` option in the "
                 "[sc-builder] group.\n"
              << "Make sure this path is a valid directory where you have write permissions and that it exists.\n"
              << "For more information, run with --help.\n";
    return EXIT_FAILURE;
  }
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
