#include <iostream>
#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>

namespace pod = boost::program_options::detail;
namespace fs = boost::filesystem;

std::map<std::string, std::string> parse_config(std::string conf) 
{
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
            if (i->string_key == "Network.Port") result.insert({"port", i->value[0]});
            if (i->string_key == "Repo.SavePeriod") result.insert({"save_period", i->value[0]});
            if (i->string_key == "Stat.UpdatePeriod") result.insert({"update_period", i->value[0]});
            if (i->string_key == "Stat.Path") result.insert({"stat_path", i->value[0]});
        }
    }
    catch(std::exception& e)    
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    result["path"].append("kb.bin/"); 
    for (auto it : result)
        if (it.second.size()-1 == '/')
            it.second = it.second.substr(0, it.second.size()-1);
    return result;
}
