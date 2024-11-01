/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-config/sc_params.hpp"

#include <sc-memory/sc_debug.hpp>

template <typename TContentType>
TContentType const & ScParams::Get(std::string const & key) const
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
TContentType const & ScParams::Get(std::string const & key, TContentType const & defaultValue) const
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
