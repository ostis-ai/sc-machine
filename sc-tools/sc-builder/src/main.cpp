/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder.hpp"

#include <iostream>

#include "sc_memory_config.hpp"

sc_int main(sc_int argc, sc_char * argv[])
try
{
  ScOptions options{argc, argv};

  if (options.Has({"help"}) || !options.Has({"input_path", "i"}) || !options.Has({"output_path", "o"}))
  {
    std::cout << "SC-BUILDER USAGE\n\n"
              << "--config|-c -- Path to configuration file\n"
              << "--input_path|-i -- Path to directory with sources\n"
              << "--output_path|-o -- Path to output directory (repository)\n"
              << "--auto_formats|-f -- Enable automatic formats info generation\n"
              << "--clear -- Flag to clear sc-memory on start\n"
              << "--help -- Display this message\n\n";
    return EXIT_SUCCESS;
  }

  BuilderParams params;
  if (options.Has({"input_path", "i"}))
    params.m_inputPath = options[{"input_path", "i"}].second;

  if (options.Has({"output_path", "o"}))
    params.m_outputPath = options[{"output_path", "o"}].second;

  if (options.Has({"enabled_ext"}))
    params.m_enabledExtPath = options[{"enabled_ext"}].second;

  params.m_autoFormatInfo = options.Has({"auto_formats", "f"}) ? SC_TRUE : SC_FALSE;

  std::string config;
  if (options.Has({"config", "c"}))
    config = options[{"config", "c"}].second;

  std::vector<std::vector<std::string>> keys = {{"verbose", "v"}, {"clear"}};
  ScParams memoryParams{options, keys};
  memoryParams.insert({"repo_path", options[{"output_path", "o"}].second});

  ScMemoryConfig memoryConfig{config, {"repo_path", "log_file"}, std::move(memoryParams)};

  Builder builder;
  return builder.Run(params, memoryConfig.GetParams()) ? EXIT_SUCCESS : EXIT_FAILURE;
}
catch (utils::ScException const & ex)
{
  SC_LOG_ERROR(ex.Message());
}
