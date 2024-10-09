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
