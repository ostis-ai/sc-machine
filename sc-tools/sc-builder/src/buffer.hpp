/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

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
