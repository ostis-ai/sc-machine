#pragma once

#include <utility>
#include <map>

#include "sc-memory/sc_memory.hpp"

#include "sc-config/sc_config.hpp"
#include "sc-core/sc_memory_params.h"
#include "sc-core/sc_memory_version.h"

using ScParams = std::map<std::string, std::string>;

class ScMemoryConfig
{
public:
  explicit ScMemoryConfig(std::string const & path, ScParams params, std::string groupName = "sc-memory")
    : m_groupName(std::move(groupName))
    , m_params(std::move(params))
  {
    ScConfig config{path};

    if (config.IsValid())
    {
      ScConfigGroup group = config[m_groupName];
      for (auto const & key : *group)
        m_params.insert({key, group[key]});
    }
  }

  explicit ScMemoryConfig(ScConfig const & config, ScParams params, std::string groupName = "sc-memory")
    : m_groupName(std::move(groupName))
    , m_params(std::move(params))
  {
    if (config.IsValid())
    {
      ScConfigGroup group = config[m_groupName];
      for (auto const & key : *group)
        m_params.insert({key, group[key]});
    }
  }

  sc_memory_params GetParams()
  {
    sc_version version = {
        SC_MACHINE_VERSION_MAJOR, SC_MACHINE_VERSION_MINOR, SC_MACHINE_VERSION_PATCH, SC_MACHINE_VERSION_SUFFIX};

    m_memoryParams.version = (sc_char const *)sc_version_string_new(&version);
    m_memoryParams.clear = m_params.count("clear") ? SC_TRUE : SC_FALSE;
    m_memoryParams.repo_path = (sc_char const *)m_params.at("repo_path").c_str();
    m_memoryParams.ext_path =
        (sc_char const *)(m_params.count("extensions_path") ? m_params.at("extensions_path").c_str() : null_ptr);
    m_memoryParams.enabled_exts = nullptr;
    m_memoryParams.save_period = m_params.count("save_period") ? std::stoi(m_params["save_period"]) : 32;
    m_memoryParams.update_period = m_params.count("update_period") ? std::stoi(m_params["update_period"]) : 16;

    m_memoryParams.debug_type = (sc_char const *)SC_MACHINE_LOG_TYPE;
    m_memoryParams.debug_mode = (sc_char const *)SC_MACHINE_LOG_MODE;
    m_memoryParams.debug_file = (sc_char const *)SC_MACHINE_LOG_DIR;

    m_memoryParams.max_threads = m_params.count("max_threads") ? std::stoi(m_params["max_threads"]) : 32;
    m_memoryParams.max_loaded_segments =
        m_params.count("max_loaded_segments") ? std::stoi(m_params["max_loaded_segments"]) : 1000;

    return m_memoryParams;
  }

  ~ScMemoryConfig()
  {
    sc_version_string_free((sc_char *)m_memoryParams.version);
  }

private:
  std::string m_groupName;
  ScParams m_params;

  sc_memory_params m_memoryParams{};
};
