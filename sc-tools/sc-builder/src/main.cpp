/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <boost/program_options.hpp>

#include "builder.hpp"

#include <iostream>

#include "sc_memory_config.hpp"

sc_int main(sc_int argc, sc_char * argv[])
try
{
  boost::program_options::options_description options_description("Builder usage");
  options_description.add_options()("help", "Display this message")("version,v", "Displays version number")(
      "input_path,i", boost::program_options::value<std::string>(), "Path to directory with sources")(
      "output_path,o", boost::program_options::value<std::string>(), "Path to output directory (repository)")(
      "extensions_path,e", boost::program_options::value<std::string>(), "Path to extensions directory")(
      "enabled_ext", boost::program_options::value<std::string>(), "Path to file with enabled extensions")(
      "clear", "Clear output directory (repository) before build")(
      "config,c", boost::program_options::value<std::string>(), "Path to configuration file for sc-memory")(
      "auto_formats,f", "Enable automatic formats info generation");

  boost::program_options::variables_map vm;
  boost::program_options::store(
      boost::program_options::command_line_parser(argc, argv).options(options_description).run(), vm);
  boost::program_options::notify(vm);

  if (vm.count("help") || !vm.count("input_path") || !vm.count("output_path"))
  {
    std::cout << options_description;
    return EXIT_SUCCESS;
  }

  BuilderParams params;
  if (vm.count("input_path"))
    params.m_inputPath = vm["input_path"].as<std::string>();

  if (vm.count("input_path"))
    params.m_outputPath = vm["output_path"].as<std::string>();

  if (vm.count("enabled_ext"))
    params.m_enabledExtPath = vm["enabled_ext"].as<std::string>();

  params.m_autoFormatInfo = vm.count("auto_formats") ? SC_TRUE : SC_FALSE;

  std::string config;
  if (vm.count("config"))
    config = vm["config"].as<std::string>();

  ScParams memoryParams;
  std::vector<std::string> keys = {"extensions_path", "verbose", "clear"};
  for (auto const & key : keys)
  {
    if (vm.count(key))
      memoryParams.insert({key, vm[key].as<std::string>()});
  }
  memoryParams.insert({"repo_path", vm["output_path"].as<std::string>()});

  ScMemoryConfig memoryConfig{config, memoryParams};

  Builder builder;
  return builder.Run(params, memoryConfig.GetParams()) ? EXIT_SUCCESS : EXIT_FAILURE;
}
catch (utils::ScException const & ex)
{
  SC_LOG_ERROR(ex.Message());
}
