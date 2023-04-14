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

  ScParams(ScParams const & object) noexcept
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
  explicit ScMemoryConfig(
      std::string const & path,
      std::unordered_set<std::string> const & pathKeys,
      std::unordered_set<std::string> const & notUsedKeys,
      ScParams const & params,
      std::string const & groupName = "sc-memory")
    : ScMemoryConfig(ScConfig(path, pathKeys, notUsedKeys), params, groupName)
  {
  }

  explicit ScMemoryConfig(ScConfig const & config, ScParams const & params, std::string groupName = "sc-memory")
    : m_params(params)
    , m_groupName(std::move(groupName))
  {
    if (config.IsValid())
    {
      ScConfigGroup group = config[m_groupName];
      for (auto const & key : *group)
      {
        std::string const & value = group[key];
        std::stringstream stream;

        m_params.insert({key, value});
      }
    }
  }

  sc_char const * GetStringByKey(std::string const & key, sc_char const defaultValue[] = nullptr)
  {
    return m_params.count(key) ? m_params.at(key).c_str() : (sc_char const *)defaultValue;
  }

  sc_int32 GetIntByKey(std::string const & key, sc_int32 const defaultValue = 0)
  {
    return m_params.count(key) ? std::stoi(m_params.at(key)) : defaultValue;
  }

  sc_bool GetBoolByKey(std::string const & key, sc_bool const defaultValue = SC_FALSE)
  {
    return m_params.count(key) ? (m_params.at(key) == "true" ? SC_TRUE : SC_FALSE) : defaultValue;
  }

  sc_bool HasKey(std::string const & key)
  {
    return m_params.count(key);
  }

  sc_memory_params GetParams()
  {
    sc_version version = {
        SC_MACHINE_VERSION_MAJOR, SC_MACHINE_VERSION_MINOR, SC_MACHINE_VERSION_PATCH, SC_MACHINE_VERSION_SUFFIX};

    m_memoryParams.version = (sc_char const *)sc_version_string_new(&version);
    m_memoryParams.clear = HasKey("clear");
    m_memoryParams.repo_path = GetStringByKey("repo_path");
    m_memoryParams.ext_path = GetStringByKey("extensions_path");
    m_memoryParams.enabled_exts = nullptr;

    m_memoryParams.max_loaded_segments = GetIntByKey("max_loaded_segments", DEFAULT_MAX_LOADED_SEGMENTS);
    m_memoryParams.max_threads = GetIntByKey("max_threads", DEFAULT_MAX_THREADS);
    m_memoryParams.max_events_and_agents_threads =
        GetIntByKey("max_events_and_agents_threads", DEFAULT_EVENTS_PROCESSORS);

    m_memoryParams.save_period = GetIntByKey("save_period", DEFAULT_SAVE_PERIOD);
    m_memoryParams.update_period = GetIntByKey("update_period", DEFAULT_UPDATE_PERIOD);

    m_memoryParams.log_type = GetStringByKey("log_type", DEFAULT_LOG_TYPE);
    m_memoryParams.log_file = GetStringByKey("log_file", DEFAULT_LOG_FILE);
    m_memoryParams.log_level = GetStringByKey("log_level", DEFAULT_LOG_LEVEL);

    m_memoryParams.init_memory_generated_upload = GetBoolByKey("init_memory_generated_upload");
    m_memoryParams.init_memory_generated_structure = GetStringByKey("init_memory_generated_structure");

    return m_memoryParams;
  }

  ~ScMemoryConfig()
  {
    sc_version_string_free((sc_char *)m_memoryParams.version);
  }

private:
  ScParams m_params;
  std::string m_groupName;

  sc_memory_params m_memoryParams{};
};
