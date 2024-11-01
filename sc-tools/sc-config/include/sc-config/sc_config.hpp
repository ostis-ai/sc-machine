/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>

extern "C"
{
#include "sc_config.h"
}

class ScConfigGroup
{
public:
  explicit ScConfigGroup(
      sc_config * config,
      std::string configPath,
      std::unordered_set<std::string> const & pathKeys,
      std::unordered_set<std::string> const & notUsedKeys,
      std::string group);

  std::string operator[](std::string const & key) const;

  std::unordered_set<std::string> operator*() const;

  ~ScConfigGroup() = default;

private:
  sc_config * m_config;
  std::string m_configPath;
  std::unordered_set<std::string> m_pathKeys;
  std::unordered_set<std::string> m_notUsedKeys;

  std::string m_group;
  std::unordered_set<std::string> m_keys;
};

class ScConfig
{
public:
  explicit ScConfig(
      std::string path,
      std::unordered_set<std::string> const & pathKeys = {},
      std::unordered_set<std::string> const & notUsedKeys = {});

  sc_bool IsValid() const;

  template <class Type>
  Type Get(std::string const & group, std::string const & key) const
  {
    sc_char * value = sc_config_get_value_string(m_instance, group.c_str(), key.c_str());
    std::stringstream stream;
    stream << std::string(value);

    Type targetValue;
    stream >> targetValue;
    return targetValue;
  }

  ScConfigGroup operator[](std::string const & group) const;

  std::string GetDirectory() const;

  ~ScConfig();

private:
  std::string m_path;
  std::unordered_set<std::string> m_pathKeys;
  std::unordered_set<std::string> m_notUsedKeys;

  sc_config * m_instance{};

  sc_bool m_result;
};
