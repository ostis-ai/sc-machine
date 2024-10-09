/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "buffer.hpp"

#include "gwf_translator_constants.hpp"

Buffer::Buffer()
  : m_value("")
{
}

Buffer & Buffer::operator<<(std::string const & string)
{
  m_value << string;
  return *this;
}

Buffer & Buffer::AddTabs(std::size_t const & count)
{
  std::string tabs(count * 4, Constants::SPACE[0]);
  m_value << tabs;
  return *this;
}

std::string Buffer::GetValue() const
{
  return m_value.str();
}
