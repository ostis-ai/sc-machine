/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-config/sc_config.hpp"

template <class Type>
Type ScConfig::Get(std::string const & group, std::string const & key) const
{
  sc_char * value = sc_config_get_value_string(m_instance, group.c_str(), key.c_str());
  std::stringstream stream;
  stream << std::string(value);

  Type targetValue;
  stream >> targetValue;
  return targetValue;
}
