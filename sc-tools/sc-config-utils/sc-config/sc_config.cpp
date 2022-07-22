/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_config.hpp"

#include <string>
#include <sstream>
#include <utility>
#include <algorithm>
#include <vector>

extern "C"
{
#include "sc-core/sc-store/sc-container/sc-string/sc_string.h"
#include "sc_config.h"
}

ScConfigGroup::ScConfigGroup(
    sc_config * config,
    std::string configPath,
    std::vector<std::string> pathKeys,
    std::string group)
  : m_config(config)
  , m_configPath(std::move(configPath))
  , m_pathKeys(std::move(pathKeys))
  , m_group(std::move(group))
{
  sc_list * keys = sc_config_get_keys(m_config);

  sc_list * element = sc_list_get_first(keys);
  while (element != null_ptr)
  {
    if (sc_str_has_prefix((sc_char *)element->data, m_group.c_str()))
    {
      std::string groupKey = std::string((sc_char *)element->data);
      m_keys.push_back(groupKey.erase(0, m_group.size() + 1));
    }

    element = element->next;
  }

  sc_list_free(keys);
}

std::string ScConfigGroup::operator[](std::string const & key) const
{
  std::string const & value = sc_config_get_value_string(m_config, m_group.c_str(), key.c_str());
  std::stringstream stream;

  auto const & it = std::find(m_pathKeys.cbegin(), m_pathKeys.cend(), key);

  if (it == m_pathKeys.cend())
    return value;

  if (value[0] == '/')
    stream << value;
  else
    stream << m_configPath << value;

  return stream.str();
}

std::vector<std::string> ScConfigGroup::operator*() const
{
  return m_keys;
}

ScConfig::ScConfig(std::string path, std::vector<std::string> paths)
  : m_path(std::move(path))
  , m_pathKeys(std::move(paths))
{
  m_result = sc_config_initialize(&m_instance, m_path.c_str());
}

sc_bool ScConfig::IsValid() const
{
  return m_result;
}

ScConfigGroup ScConfig::operator[](std::string const & group) const
{
  return ScConfigGroup(this->m_instance, this->GetDirectory(), this->m_pathKeys, group);
}

ScConfig::~ScConfig()
{
  sc_config_shutdown(m_instance);
  m_instance = nullptr;
}
