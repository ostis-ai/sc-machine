#pragma once

#include <string>

extern "C"
{
#include "sc_config.h"
}

using sc_list = GList;

class ScConfigGroup
{
public:
  explicit ScConfigGroup(sc_config * config, std::string group)
    : m_config(config), m_group(std::move(group))
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
  }

  std::string operator[](std::string const & key)
  {
    return sc_config_get_value_string(m_config, m_group.c_str(), key.c_str());
  }

  std::vector<std::string> operator*()
  {
    return m_keys;
  }

  ~ScConfigGroup() = default;

private:
  sc_config * m_config;
  std::string m_group;
  std::vector<std::string> m_keys;
};

class ScConfig
{
public:
  friend ScConfigGroup;

  explicit ScConfig(std::string path)
    : m_path(std::move(path))
  {
    sc_bool const result = sc_config_initialize(&m_instance, m_path.c_str());
    SC_ASSERT(result == SC_TRUE, ("Invalid config path"));
  }

  template <class Type>
  Type Get(std::string const & group, std::string const & key)
  {
    if (typeid(Type) == typeid(std::string) || typeid(Type) == typeid(sc_char *))
      return sc_config_get_value_string(m_instance, group.c_str(), key.c_str());
    else if (typeid(Type) == typeid(sc_int))
      return sc_config_get_value_int(m_instance, group.c_str(), key.c_str());
    else if (typeid(Type) == typeid(float))
      return sc_config_get_value_float(m_instance, group.c_str(), key.c_str());
    else if (typeid(Type) == typeid(sc_bool))
      return sc_config_get_value_boolean(m_instance, group.c_str(), key.c_str());
    else
      return 0;
  }

  ScConfigGroup operator[](std::string const & group)
  {
    return ScConfigGroup(this->m_instance, group);
  }

  ~ScConfig()
  {
    sc_config_shutdown(m_instance);
    m_instance = nullptr;
  }

private:
  std::string m_path;
  sc_config * m_instance{};
};
