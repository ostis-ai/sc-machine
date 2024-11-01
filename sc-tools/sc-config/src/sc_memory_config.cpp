/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-config/sc_memory_config.hpp"

#include <sc-memory/sc_memory.hpp>

extern "C"
{
#include <sc-core/sc_memory_version.h>
}

#include "sc-config/sc_options.hpp"
#include "sc-config/sc_config.hpp"

ScParams::ScParams(ScOptions const & options, std::vector<std::vector<std::string>> const & keysSet)
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

ScParams::ScParams() = default;

void ScParams::Insert(std::pair<std::string, std::string> const & pair)
{
  m_params.insert(pair);
}

sc_bool ScParams::Has(std::string const & key) const
{
  return m_params.count(key);
}

ScMemoryConfig::ScMemoryConfig(ScConfig const & config, ScParams const & params, std::string groupName)
  : m_params(params)
  , m_groupName(std::move(groupName))
{
  if (!config.IsValid())
    return;

  ScConfigGroup group = config[m_groupName];
  for (auto const & key : *group)
  {
    std::string value = group[key];
    value = !value.empty() && value[0] == '\"' ? value.substr(1, value.length() - 2) : value;

    m_params.Insert({key, value});
  }
}

sc_char const * ScMemoryConfig::GetStringByKey(std::string const & key, sc_char const * defaultValue)
{
  return m_params.Has(key) ? m_params.Get<std::string>(key).c_str() : defaultValue;
}

sc_int32 ScMemoryConfig::GetIntByKey(std::string const & key, sc_int32 const defaultValue)
{
  return m_params.Get(key, defaultValue);
}

bool ScMemoryConfig::GetBoolByKey(std::string const & key, bool const defaultValue)
{
  return m_params.Has(key) ? (m_params.Get<std::string>(key) == "true" ? true : false) : defaultValue;
}

bool ScMemoryConfig::HasKey(std::string const & key)
{
  return m_params.Has(key);
}

sc_memory_params ScMemoryConfig::GetParams()
{
  m_memoryParams.version = {
      SC_MACHINE_VERSION_MAJOR, SC_MACHINE_VERSION_MINOR, SC_MACHINE_VERSION_PATCH, SC_MACHINE_VERSION_SUFFIX};

  m_memoryParams.clear = HasKey("clear");
  m_memoryParams.repo_path = GetStringByKey("kb_binaries");
  m_memoryParams.ext_path = HasKey("extensions") ? GetStringByKey("extensions") : nullptr;
  m_memoryParams.enabled_exts = nullptr;

  m_memoryParams.max_loaded_segments = GetIntByKey("max_loaded_segments", DEFAULT_MAX_LOADED_SEGMENTS);

  m_memoryParams.limit_max_threads_by_max_physical_cores =
      GetBoolByKey("limit_max_threads_by_max_physical_cores", DEFAULT_LIMIT_MAX_THREADS_BY_MAX_PHYSICAL_CORES);
  m_memoryParams.max_events_and_agents_threads =
      GetIntByKey("max_events_and_agents_threads", DEFAULT_MAX_EVENTS_AND_AGENTS_THREADS);

  m_memoryParams.dump_memory = GetBoolByKey("dump_memory", DEFAULT_DUMP_MEMORY);
  if (HasKey("save_period"))
  {
    SC_LOG_WARNING(
        "Option `save_period` is deprecated in sc-machine 0.9.0. It will be removed in sc-machine 0.10.0. Use option "
        "`dump_memory_period` instead of.");
    m_memoryParams.save_period = m_memoryParams.dump_memory_period =
        GetIntByKey("save_period", DEFAULT_DUMP_MEMORY_PERIOD);
  }
  if (HasKey("dump_memory_period"))
    m_memoryParams.save_period = m_memoryParams.dump_memory_period =
        GetIntByKey("dump_memory_period", DEFAULT_DUMP_MEMORY_PERIOD);

  m_memoryParams.dump_memory_statistics = GetBoolByKey("dump_memory", DEFAULT_DUMP_MEMORY_STATISTICS);
  if (HasKey("update_period"))
  {
    SC_LOG_WARNING(
        "Option `update_period` is deprecated in sc-machine 0.9.0. It will be removed in sc-machine 0.10.0. Use "
        "option `dump_memory_statistics_period` instead of.");
    m_memoryParams.update_period = m_memoryParams.dump_memory_statistics_period =
        GetIntByKey("update_period", DEFAULT_DUMP_MEMORY_STATISTICS_PERIOD);
  }
  if (HasKey("dump_memory_statistics_period"))
    m_memoryParams.update_period = m_memoryParams.dump_memory_statistics_period =
        GetIntByKey("dump_memory_statistics_period", DEFAULT_DUMP_MEMORY_STATISTICS_PERIOD);

  m_memoryParams.log_type = GetStringByKey("log_type", DEFAULT_LOG_TYPE);
  m_memoryParams.log_file = GetStringByKey("log_file", DEFAULT_LOG_FILE);
  m_memoryParams.log_level = GetStringByKey("log_level", DEFAULT_LOG_LEVEL);

  m_memoryParams.init_memory_generated_upload = GetBoolByKey("init_memory_generated_upload");
  m_memoryParams.init_memory_generated_structure = GetStringByKey("init_memory_generated_structure");

  m_memoryParams.user_mode = GetBoolByKey("user_mode");

  m_memoryParams.max_strings_channels = GetIntByKey("max_strings_channels", DEFAULT_MAX_STRINGS_CHANNELS);
  m_memoryParams.max_strings_channel_size = GetIntByKey("max_strings_channel_size", DEFAULT_MAX_STRINGS_CHANNEL_SIZE);
  m_memoryParams.max_searchable_string_size =
      GetIntByKey("max_searchable_string_size", DEFAULT_MAX_SEARCHABLE_STRING_SIZE);
  m_memoryParams.term_separators = GetStringByKey("term_separators", DEFAULT_TERM_SEPARATORS);
  m_memoryParams.search_by_substring = GetBoolByKey("search_by_substring", DEFAULT_SEARCH_BY_SUBSTRING);

  return m_memoryParams;
}
