#include "builder.h"

#include <boost/program_options.hpp>

int main(int argc, char *argv[])
{

    boost::program_options::options_description options_description("Builder usage");
    options_description.add_options()
            ("help", "Display this message")
            ("version", "Displays version number")
            ("input-path,i", boost::program_options::value<std::string>(), "Path to directory with sources")
            ("output-path,o", boost::program_options::value<std::string>(), "Path to output directory (repository)")
            ("clear-output,c", "Clear output directory (repository) before build")
            ("auto-formats,f", "Enable automatic formats info generation");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options_description).run(), vm);
    boost::program_options::notify(vm);

    if (vm.count("help"))
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

    if (vm.count("clear-output"))
        params.clearOutput = true;

    if (vm.count("auto-formats"))
        params.autoFormatInfo = true;


    Builder builder;
    builder.initialize();
    builder.run(params);

    return 0;
}
