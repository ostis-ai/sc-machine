/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_debug.hpp"

#include <utility>

namespace utils
{
ScException::ScException(std::string description, std::string msg)
  : m_description(std::move(description))
  , m_msg(std::move(msg))
{
}

ScException::~ScException() noexcept = default;

sc_char const * ScException::Description() const noexcept
{
  return m_description.c_str();
}

sc_char const * ScException::Message() const noexcept
{
  return m_msg.c_str();
}

}  // namespace utils
