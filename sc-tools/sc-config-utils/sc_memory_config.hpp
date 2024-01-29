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

#include "sc_options.hpp"
#include "sc-core/sc_memory_params.h"
#include "sc-config/sc_config.hpp"

class ScParams
{
public:
  explicit ScParams(ScOptions const & options, std::vector<std::vector<std::string>> const & keysSet);

  ScParams(ScParams const & object) noexcept;

  void insert(std::pair<std::string, std::string> const & pair);

  std::string const & at(std::string const & key) const;

  bool count(std::string const & key) const;

private:
  std::map<std::string, std::string> m_params;
};

class ScMemoryConfig
{
public:
  explicit ScMemoryConfig(ScConfig const & config, ScParams const & params, std::string groupName = "sc-memory");

  sc_char const * GetStringByKey(std::string const & key, sc_char const defaultValue[] = nullptr);

  sc_int32 GetIntByKey(std::string const & key, sc_int32 const defaultValue = 0);

  sc_bool GetBoolByKey(std::string const & key, sc_bool const defaultValue = SC_FALSE);

  sc_bool HasKey(std::string const & key);

  sc_memory_params GetParams();

private:
  ScParams m_params;
  std::string m_groupName;

  sc_memory_params m_memoryParams{};
};
