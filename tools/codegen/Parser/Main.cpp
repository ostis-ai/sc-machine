#include "Precompiled.hpp"

#include "ReflectionOptions.hpp"
#include "ReflectionParser.hpp"

#include "Switches.hpp"

#include <chrono>
#include <iostream>

void parse(const po::variables_map &cmdLine);

int main(int argc, char *argv[])
{
    auto start = std::chrono::system_clock::now();

    // misc initialization
    {
        // path to the executable
        auto exeDir = fs::path(argv[ 0 ]);

        // set the working directory to the executable directory
        fs::current_path(exeDir.parent_path());
    }

    // parse command line
    try 
    {
        po::options_description program("SC Reflection Parser");

        program.add_options()
        (
            SWITCH_OPTION(Help), 
            "Displays help information." 
       )
        (
            SWITCH_OPTION(TargetName), 
            po::value<std::string>()->required(),
            "Input target project name." 
       )
        (
            SWITCH_OPTION(Input), 
            po::value<std::string>()->required(), 
            "Source path to parse headers in." 
       )
        (
            SWITCH_OPTION(Output), 
            po::value<std::string>()->required(), 
            "Output path for generated headers." 
       )
        (
            SWITCH_OPTION(TemplateDirectory), 
            po::value<std::string>()->default_value("Templates/"), 
            "Directory that contains the mustache templates." 
       )
        (
            SWITCH_OPTION(CompilerFlags), 
            po::value<std::vector<std::string>>()->multitoken()->required(),
            "Optional list of flags to pass to the compiler." 
       );

        po::variables_map cmdLine;

        po::store(po::parse_command_line(argc, argv, program), cmdLine);

        if (cmdLine.count("help")) 
        {
            std::cout << program << std::endl;

            return EXIT_SUCCESS;
        }

        po::notify(cmdLine);

        parse(cmdLine);
    }
    catch (std::exception &e) 
    {
        utils::FatalError(e.what());
    }
    catch (...) 
    {
        utils::FatalError("Unhandled exception occurred!");
    }

    auto duration = std::chrono::system_clock::now() - start;

    std::cout << "Completed in " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() 
              << "ms" << std::endl;

    return EXIT_SUCCESS;
}

void parse(const po::variables_map &cmdLine)
{
    ReflectionOptions options;

    options.targetName = 
        cmdLine.at(kSwitchTargetName).as<std::string>();

    options.inputPath = 
        cmdLine.at(kSwitchInput).as<std::string>();

    options.outputPath = 
        cmdLine.at(kSwitchOutput).as<std::string>();

    // default arguments
    options.arguments =
    { {
        "-x",
        "c++",
        "-std=c++11",
        "-D__SC_REFLECTION_PARSER__"
    } };

    if (cmdLine.count(kSwitchCompilerFlags))
    {
        auto flags = cmdLine.at(kSwitchCompilerFlags).as<std::vector<std::string>>();

        for (auto &flag : flags) 
            options.arguments.emplace_back(flag.c_str());
    }

    options.templateDirectory = cmdLine.at(kSwitchTemplateDirectory).as<std::string>();
    
    std::cout << "Generate reflection code for \"" 
              << options.targetName << "\"" 
              << std::endl;

    ReflectionParser parser(options);

    parser.Parse();
}