#pragma once

#include <sstream>

class Buffer
{
public:
  Buffer();

  Buffer & operator<<(std::string const & string);
  Buffer & AddTabs(std::size_t const & count);

  std::string GetValue() const;

private:
  std::stringstream m_value;
};
