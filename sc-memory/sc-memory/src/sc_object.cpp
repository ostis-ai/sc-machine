/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_object.hpp"

ScObject::ScObject() = default;

ScObject & ScObject::operator=(ScObject const & other) = default;

ScObject::~ScObject() = default;

std::string ScObject::GetName() const
{
  if (m_name.empty())
    m_name = Demangle(typeid(*this).name());

  return m_name;
}

std::string ScObject::Demangle(std::string const & mangledName)
{
  std::string demangled;
  size_t i = 0;

  if (mangledName[0] == 'N')
    ++i;

  while (i < mangledName.size() && std::isdigit(mangledName[i]))
  {
    // Read the length of the next part
    size_t len = 0;
    while (i < mangledName.size() && std::isdigit(mangledName[i]))
    {
      len = len * 10 + (mangledName[i] - '0');
      ++i;
    }

    // Extract the part of the name
    if (i + len <= mangledName.size())
    {
      if (!demangled.empty())
        demangled += "::";

      demangled += mangledName.substr(i, len);
      i += len;
    }
    else
    {
      // If the length is invalid, return the original mangled name
      return mangledName;
    }
  }

  return demangled;
}
