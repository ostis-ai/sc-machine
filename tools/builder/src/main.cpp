/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
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

    Builder builder;
    builder.initialize();
    builder.run(params);

    return 0;
}
