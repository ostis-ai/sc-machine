/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-config/sc_config.hpp"

#include <string>
#include <utility>
#include <algorithm>
#include <unordered_set>

extern "C"
{
#include <glib.h>
#include <sc-core/sc-container/sc_string.h>

#include "sc-config/sc_config.h"
}

ScConfigGroup::ScConfigGroup(
    sc_config * config,
    std::string configPath,
    std::unordered_set<std::string> const & pathKeys,
    std::unordered_set<std::string> const & notUsedKeys,
    std::string group)
  : m_config(config)
  , m_configPath(std::move(configPath))
  , m_pathKeys(pathKeys)
  , m_notUsedKeys(notUsedKeys)
  , m_group(std::move(group))
{
  if (m_config == nullptr)
    return;

  GList * keys = sc_config_get_keys(m_config);

  GList * element = sc_list_get_first(keys);
  while (element != null_ptr)
  {
    if (sc_str_has_prefix((sc_char *)element->data, m_group.c_str()))
    {
      std::string groupKey = std::string((sc_char *)element->data);

      std::string const & key = groupKey.erase(0, m_group.size() + 1);
      if (m_notUsedKeys.find(key) == m_notUsedKeys.cend())
        m_keys.insert(key);
    }

    element = element->next;
  }

  sc_list_free(keys);
}

std::string ScConfigGroup::operator[](std::string const & key) const
{
  sc_char * value = sc_config_get_value_string(m_config, m_group.c_str(), key.c_str());
  if (value == nullptr)
    return "";

  std::stringstream stream;

  auto const & it = m_pathKeys.find(key);
  if (it == m_pathKeys.cend())
    return value;

  if (value[0] == '/')
    stream << value;
  else
    stream << m_configPath << value;

  return stream.str();
}

std::unordered_set<std::string> ScConfigGroup::operator*() const
{
  return m_keys;
}

ScConfig::ScConfig(
    std::string path,
    std::unordered_set<std::string> const & pathKeys,
    std::unordered_set<std::string> const & notUsedKeys)
  : m_path(std::move(path))
  , m_pathKeys(pathKeys)
  , m_notUsedKeys(notUsedKeys)
{
  m_result = sc_config_initialize(&m_instance, m_path.c_str());
}

sc_bool ScConfig::IsValid() const
{
  return m_result;
}

ScConfigGroup ScConfig::operator[](std::string const & group) const
{
  return ScConfigGroup(this->m_instance, this->GetDirectory(), this->m_pathKeys, this->m_notUsedKeys, group);
}

ScConfig::~ScConfig()
{
  sc_config_shutdown(m_instance);
  m_instance = nullptr;
}
