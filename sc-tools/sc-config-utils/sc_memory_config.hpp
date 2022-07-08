/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>
#include <map>

#include "sc-memory/sc_memory.hpp"

#include "sc-config/sc_config.hpp"
#include "sc_options.hpp"

#include "sc-core/sc_memory_params.h"
#include "sc-core/sc_memory_version.h"

class ScParams
{
public:
  explicit ScParams(ScOptions const & options, std::vector<std::vector<std::string>> const & keysSet)
  {
    for (auto const & keys : keysSet)
    {
      if (options.Has(keys))
      {
        auto const & result = options[keys].second;
        for (auto const & key : keys)
          m_params.insert({key, result});
      }
    }
  }

  ScParams(ScParams && object) noexcept
  {
    m_params = object.m_params;
  }

  void insert(std::pair<std::string, std::string> const & pair)
  {
    m_params.insert(pair);
  }

  std::string const & at(std::string const & key) const
  {
    if (m_params.count(key))
      return m_params.at(key);

    static std::string empty;
    return empty;
  }

  bool count(std::string const & key) const
  {
    return m_params.count(key);
  }

private:
  std::map<std::string, std::string> m_params;
};

class ScMemoryConfig
{
public:
  explicit ScMemoryConfig(std::string const & path, ScParams params, std::string groupName = "sc-memory")
    : ScMemoryConfig(ScConfig(path), std::move(params), std::move(groupName))
  {
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
    m_memoryParams.repo_path = m_params.at("repo_path").c_str();
    m_memoryParams.ext_path =
        m_params.count("extensions_path") ? m_params.at("extensions_path").c_str() : (sc_char const *)null_ptr;
    m_memoryParams.enabled_exts = nullptr;
    m_memoryParams.save_period = m_params.count("save_period") ? std::stoi(m_params.at("save_period")) : 32000;
    m_memoryParams.update_period = m_params.count("update_period") ? std::stoi(m_params.at("update_period")) : 16000;

    m_memoryParams.debug_type = (sc_char const *)SC_MACHINE_LOG_TYPE;
    m_memoryParams.debug_mode = (sc_char const *)SC_MACHINE_LOG_MODE;
    m_memoryParams.debug_file = (sc_char const *)SC_MACHINE_LOG_DIR;

    m_memoryParams.max_threads = m_params.count("max_threads") ? std::stoi(m_params.at("max_threads")) : 32;
    m_memoryParams.max_loaded_segments =
        m_params.count("max_loaded_segments") ? std::stoi(m_params.at("max_loaded_segments")) : 1000;

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
