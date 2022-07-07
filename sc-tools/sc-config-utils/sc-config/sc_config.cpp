/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_config.hpp"

#include <string>
#include <vector>

extern "C"
{
#include "sc_config.h"
}

ScConfigGroup::ScConfigGroup(sc_config * config, std::string group)
  : m_config(config)
  , m_group(std::move(group))
{
  sc_list * keys = g_hash_table_get_keys(m_config);

  sc_list * element = g_list_first(keys);
  while (element != null_ptr)
  {
    if (g_str_has_prefix((sc_char *)element->data, m_group.c_str()))
    {
      std::string groupKey = std::string((sc_char *)element->data);
      m_keys.push_back(groupKey.erase(0, m_group.size() + 1));
    }

    element = element->next;
  }

  g_list_free(keys);
}

std::string ScConfigGroup::operator[](std::string const & key) const
{
  return sc_config_get_value_string(m_config, m_group.c_str(), key.c_str());
}

std::vector<std::string> ScConfigGroup::operator*() const
{
  return m_keys;
}

ScConfig::ScConfig(std::string path)
  : m_path(std::move(path))
{
  m_result = sc_config_initialize(&m_instance, m_path.c_str());
}

sc_bool ScConfig::IsValid() const
{
  return m_result;
}

ScConfigGroup ScConfig::operator[](std::string const & group) const
{
  return ScConfigGroup(this->m_instance, group);
}

ScConfig::~ScConfig()
{
  sc_config_shutdown(m_instance);
  m_instance = nullptr;
}
