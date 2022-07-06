#pragma once

#include <string>
#include <vector>

extern "C"
{
#include "sc_config.h"
}

using sc_list = GList;

class ScConfigGroup
{
public:
  explicit ScConfigGroup(sc_config * config, std::string group);

  std::string operator[](std::string const & key) const;

  std::vector<std::string> operator*() const;

  ~ScConfigGroup() = default;

private:
  sc_config * m_config;
  std::string m_group;
  std::vector<std::string> m_keys;
};

class ScConfig
{
public:
  explicit ScConfig(std::string path);

  sc_bool IsValid() const;

  template <class Type>
  Type Get(std::string const & group, std::string const & key) const
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

  ScConfigGroup operator[](std::string const & group) const;

  ~ScConfig();

private:
  std::string m_path;
  sc_config * m_instance{};

  sc_bool m_result;
};