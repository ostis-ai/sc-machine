/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <utility>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>

#include <sc-memory/sc_debug.hpp>

#include "sc_options.hpp"
#include "sc-core/sc_memory_params.h"
#include "sc-config/sc_config.hpp"

class ScMemoryConfig;

class ScParams
{
public:
  friend class ScMemoryConfig;

  explicit ScParams(ScOptions const & options, std::vector<std::vector<std::string>> const & keysSet);

  ScParams();

  ScParams(ScParams const & object) noexcept;

  void Insert(std::pair<std::string, std::string> const & pair);

  template <typename TContentType>
  TContentType const & Get(std::string const & key) const
  {
    if constexpr (std::is_same_v<TContentType, std::string>)
    {
      if (m_params.count(key) == SC_FALSE)
        SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Parameter value not found by `" << key << "`");

      return m_params.at(key);
    }

    std::stringstream streamString;
    if (m_params.count(key))
      streamString << m_params.at(key);

    static TContentType value;
    streamString >> value;
    return value;
  }

  template <typename TContentType>
  TContentType const & Get(std::string const & key, TContentType const & defaultValue) const
  {
    if constexpr (std::is_same_v<TContentType, std::string>)
      return m_params.count(key) ? m_params.at(key) : defaultValue;

    std::stringstream streamString;
    if (m_params.count(key))
      streamString << m_params.at(key);
    else
      streamString << defaultValue;

    static TContentType value;
    streamString >> value;
    return value;
  }

  sc_bool Has(std::string const & key) const;

private:
  std::map<std::string, std::string> m_params;
};

class ScMemoryConfig
{
public:
  explicit ScMemoryConfig(ScConfig const & config, ScParams const & params, std::string groupName = "sc-memory");

  sc_bool HasKey(std::string const & key);

  sc_memory_params GetParams();

private:
  ScParams m_params;
  std::string m_groupName;

  sc_memory_params m_memoryParams{};

  sc_char const * GetStringByKey(std::string const & key, sc_char const * defaultValue = nullptr);

  sc_int32 GetIntByKey(std::string const & key, sc_int32 const defaultValue = 0);

  sc_bool GetBoolByKey(std::string const & key, sc_bool const defaultValue = SC_FALSE);
};
