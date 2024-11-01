/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>

extern "C"
{
#include <sc-core/sc_memory_params.h>
}

#include "sc-config/sc_params.hpp"

class ScOptions;
class ScConfig;

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

#include "sc-config/_template/sc_memory_config.tpp"
