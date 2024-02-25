/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_builder_setup.hpp"

#include <iostream>

#include "builder.hpp"

void PrintStartMessage()
{
  std::cout << "SC-BUILDER USAGE\n\n"
            << "--config|-c -- Path to configuration file\n"
            << "--input_path|-i -- Path to directory with sources\n"
            << "--output_path|-o -- Path to output directory (repository)\n"
            << "--auto_formats|-f -- Enable automatic formats info generation\n"
            << "--clear -- Flag to clear sc-memory on start\n"
            << "--help -- Display this message\n\n";
}

sc_int BuildAndRunBuilder(sc_int argc, sc_char * argv[])
try
{
  ScOptions options{argc, argv};

  if (options.Has({"help"}))
  {
    PrintStartMessage();
    return EXIT_SUCCESS;
  }

  BuilderParams params;
  if (options.Has({"input_path", "i"}))
    params.m_inputPath = options[{"input_path", "i"}].second;

  if (options.Has({"output_path", "o"}))
    params.m_outputPath = options[{"output_path", "o"}].second;

  if (options.Has({"enabled_ext"}))
    params.m_enabledExtPath = options[{"enabled_ext"}].second;

  params.m_autoFormatInfo = options.Has({"auto_formats", "f"});

  std::string configPath;
  if (options.Has({"config", "c"}))
    configPath = options[{"config", "c"}].second;

  ScParams memoryParams{options, {{"verbose", "v"}, {"clear"}}};
  if (!params.m_outputPath.empty())
    memoryParams.Insert({"repo_path", params.m_outputPath});

  ScConfig config{configPath, {"repo_path", "log_file", "input_path"}, {"extensions_path"}};
  ScMemoryConfig memoryConfig{config, memoryParams};

  sc_memory_params formedMemoryParams = memoryConfig.GetParams();
  if (params.m_inputPath.empty())
    params.m_inputPath = config["sc-builder"]["input_path"];

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
  SC_LOG_ERROR(ex.Message());
  return EXIT_FAILURE;
}
