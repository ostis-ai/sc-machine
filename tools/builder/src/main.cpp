/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder.h"
#include <iostream>

#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{

    boost::program_options::options_description options_description("Builder usage");
	options_description.add_options()
		("help", "Display this message")
		("version", "Displays version number")
		("input-path,i", boost::program_options::value<std::string>(), "Path to directory with sources")
		("output-path,o", boost::program_options::value<std::string>(), "Path to output directory (repository)")
		("extension-path,e", boost::program_options::value<std::string>(), "Path to extensions directory")
		("clear-output,c", "Clear output directory (repository) before build")
		("settings,s", boost::program_options::value<std::string>(), "Path to configuration file for sc-memory")
		("auto-formats,f", "Enable automatic formats info generation")
		("show-filenames,v", "Enable processing filnames printing");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options_description).run(), vm);
    boost::program_options::notify(vm);

    if (vm.count("help") || !vm.count("input-path") || !vm.count("output-path"))
    {
        std::cout << options_description;
        return 0;
    }

    BuilderParams params;
    params.clearOutput = false;
    params.autoFormatInfo = false;

    if (vm.count("input-path"))
        params.inputPath = vm["input-path"].as<std::string>();

    if (vm.count("output-path"))
        params.outputPath = vm["output-path"].as<std::string>();

    if (vm.count("extension-path"))
        params.extensionsPath = vm["extension-path"].as<std::string>();

    if (vm.count("clear-output"))
        params.clearOutput = true;

    if (vm.count("auto-formats"))
        params.autoFormatInfo = true;

    if (vm.count("settings"))
        params.configFile = vm["settings"].as<std::string>();

	if (vm.count("show-filenames"))
		params.showFileNames = true;

    Builder builder;
    builder.initialize();
    builder.run(params);

    return builder.hasErrors() ? EXIT_SUCCESS : EXIT_FAILURE;
}
