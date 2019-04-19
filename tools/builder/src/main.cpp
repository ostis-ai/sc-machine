/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <boost/program_options.hpp>

#include "builder.hpp"

#include <iostream>

int main(int argc, char *argv[]) try
{

  boost::program_options::options_description options_description("Builder usage");
  options_description.add_options()
      ("help", "Display this message")
      ("version", "Displays version number")
      ("input-path,i", boost::program_options::value<std::string>(), "Path to directory with sources")
      ("output-path,o", boost::program_options::value<std::string>(), "Path to output directory (repository)")
      ("extension-path,e", boost::program_options::value<std::string>(), "Path to extensions directory")
      ("enabled-ext", boost::program_options::value<std::string>(), "Path to file with enabled extensions")
      ("clear-output,c", "Clear output directory (repository) before build")
      ("settings,s", boost::program_options::value<std::string>(), "Path to configuration file for sc-memory")
      ("auto-formats,f", "Enable automatic formats info generation");

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options_description).run(), vm);
  boost::program_options::notify(vm);

  if (vm.count("help") || !vm.count("input-path") || !vm.count("output-path"))
  {
    std::cout << options_description;
    return 0;
  }

  BuilderParams params;
  params.m_clearOutput = false;
  params.m_autoFormatInfo = false;

  if (vm.count("input-path"))
    params.m_inputPath = vm["input-path"].as<std::string>();

  if (vm.count("output-path"))
    params.m_outputPath = vm["output-path"].as<std::string>();

  if (vm.count("extension-path"))
    params.m_extensionsPath = vm["extension-path"].as<std::string>();

  if (vm.count("clear-output"))
    params.m_clearOutput = true;

  if (vm.count("auto-formats"))
    params.m_autoFormatInfo = true;

  if (vm.count("settings"))
    params.m_configFile = vm["settings"].as<std::string>();

  if (vm.count("enabled-ext"))
    params.m_enabledExtPath = vm["enabled-ext"].as<std::string>();

  Builder builder;

  return builder.Run(params) ? EXIT_SUCCESS : EXIT_FAILURE;
}
catch (utils::ScException const & ex)
{
  SC_LOG_ERROR(ex.Message());
}
