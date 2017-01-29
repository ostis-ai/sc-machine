/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_debug.hpp"

namespace utils
{

  ScException::ScException(std::string const & description)
    : m_description(description)
  {

  }

  ScException::~ScException() throw()
  {

  }

  const char* ScException::Message() const throw()
  {
    return m_description.c_str();
  }

} // namespace utils