/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "sc_debug.hpp"

namespace utils
{

  ScException::ScException(std::string const & description, std::string const & msg)
    : m_description(description)
    , m_msg(msg)
  {

  }

  ScException::~ScException() throw()
  {

  }

  const char * ScException::Description() const throw()
  {
    return m_description.c_str();
  }

  const char * ScException::Message() const throw()
  {
    return m_msg.c_str();
  }

} // namespace utils
