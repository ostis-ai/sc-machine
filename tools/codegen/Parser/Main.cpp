#include "Precompiled.hpp"

#include "ReflectionOptions.hpp"
#include "ReflectionParser.hpp"

#include "Switches.hpp"

#include <chrono>
#include <iostream>

void parse(std::string const & appName, const po::variables_map &cmdLine)
{
	ReflectionOptions options;

	options.generatorPath = appName;
	options.targetName = cmdLine.at(kSwitchTargetName).as<std::string>();
	options.inputPath = cmdLine.at(kSwitchInput).as<std::string>();
	options.outputPath = cmdLine.at(kSwitchOutput).as<std::string>();
	options.buildDirectory = cmdLine.at(kSwitchBuildDirectory).as<std::string>();

	// default arguments
	options.arguments =
	{ {
			"-analyzer-display-progress",
			"-x",
			"c++",
            "-D__SC_REFLECTION_PARSER__",
            "-std=c++11"
		} };

    if (cmdLine.count(kSwitchCompilerFlags))
    {
        auto flagsList = cmdLine.at(kSwitchCompilerFlags).as<std::vector<std::string>>();

        for (std::string & flags : flagsList)
        {
            while (1)
            {
                size_t pos = flags.find(";");
                if (pos == std::string::npos)
                    break;
                std::string f = flags.substr(0, pos);
                options.arguments.emplace_back(f);

                flags = flags.substr(pos + 1);
            }

        }
        //for (auto & flag : flags)
        //    options.arguments.emplace_back(flag.c_str());
    }

	std::cout << "Generate reflection code for \"" << options.targetName << "\"" << std::endl;

	ReflectionParser parser(options);

	try
	{
		parser.Parse();
	}
	catch (Exception e)
	{
		std::cerr << "Error: " << e.GetDescription() << std::endl;
	}
}

int main(int argc, char *argv[])
{
    auto start = std::chrono::system_clock::now();

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
            SWITCH_OPTION(BuildDirectory), 
            po::value<std::string>()->default_value("./build"), 
            "Directory that contains the build intermediate files." 
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

		std::string const appName = argv[0];
        parse(appName, cmdLine);
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

