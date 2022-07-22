#include "ReflectionOptions.hpp"
#include "ReflectionParser.hpp"

#include "MetaUtils.hpp"

#include <chrono>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

void parse(std::string const & appName, boost::program_options::variables_map const & cmdLine)
{
  ReflectionOptions options;

  options.generatorPath = appName;
  options.targetName = cmdLine.at("target").as<std::string>();
  options.inputPath = cmdLine.at("source").as<std::string>();
  options.outputPath = cmdLine.at("output").as<std::string>();
  options.buildDirectory = cmdLine.at("build_dir").as<std::string>();
  options.displayDiagnostic = (cmdLine.count("debug") > 0);
  options.useCache = (cmdLine.count("cache") > 0);

  // default arguments
  options.arguments = {
      {"-x",
       "c++",
       "-D__SC_REFLECTION_PARSER__",
       "-std=c++17",
       "-Wno-pragma-once-outside-header",
       "-Wno-nullability-completeness"}};

  if (cmdLine.count("flags"))
  {
    auto flagsList = cmdLine.at("flags").as<std::vector<std::string>>();

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

      if (!flags.empty())
        options.arguments.emplace_back(flags);
    }
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
    parser.ResetCache();
  }
}

int main(int argc, char * argv[])
{
  auto start = std::chrono::system_clock::now();

  // parse command line
  try
  {
    boost::program_options::options_description program("SC Reflection Parser");

    program.add_options()("help,h", "Displays help information.")(
        "target,t", boost::program_options::value<std::string>()->required(), "Input target project name.")(
        "source,i", boost::program_options::value<std::string>()->required(), "Source path to parse headers in.")(
        "output,o", boost::program_options::value<std::string>()->required(), "Output path for generated headers.")(
        "build_dir,b",
        boost::program_options::value<std::string>()->default_value("./build"),
        "Directory that contains the build intermediate files.")(
        "flags,f",
        boost::program_options::value<std::vector<std::string>>()->multitoken()->required(),
        "Optional list of flags to pass to the compiler.")(
        "debug,d", boost::program_options::value<bool>()->implicit_value(false), "Display compiler errors")(
        "cache,c", boost::program_options::value<bool>()->implicit_value(false), "Force cache usage");

    boost::program_options::variables_map cmdLine;

    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, program), cmdLine);

    if (cmdLine.count("help"))
    {
      std::cout << program << std::endl;

      return EXIT_SUCCESS;
    }

    boost::program_options::notify(cmdLine);

    std::string const appName = argv[0];
    parse(appName, cmdLine);
  }
  catch (std::exception & e)
  {
    utils::FatalError(e.what());
  }
  catch (...)
  {
    utils::FatalError("Unhandled exception occurred!");
  }

  auto duration = std::chrono::system_clock::now() - start;

  std::cout << "Completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"
            << std::endl;

  return EXIT_SUCCESS;
}
