#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>

namespace pod = boost::program_options::detail;
namespace fs = boost::filesystem;
namespace dll = boost::dll;

std::map<std::string, std::string> parse_config(std::string conf) 
{
    // fs::path ostis_root = dll::program_location().remove_filename().parent_path();
    fs::path conf_directory(conf);
    conf_directory = conf_directory.parent_path();

    std::ifstream config(conf);
    if(!config)
    {
        std::cerr << "error" << std::endl;
        return std::map<std::string, std::string>();
    }

    std::set<std::string> options;
    options.insert("*");
    std::map<std::string, std::string> result;
    
    try
    {      
        for (pod::config_file_iterator i(config, options), e; i != e; ++i)
        {
	    if (i->string_key == "Repo.Path") result.insert({"path", (conf_directory/i->value[0]).string()});
	    if (i->string_key == "Extensions.Directory") result.insert({"ext", (conf_directory/i->value[0]).string()});
        }
    }
    catch(std::exception& e)    
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return result;
}
