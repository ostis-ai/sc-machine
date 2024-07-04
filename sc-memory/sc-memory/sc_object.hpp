/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>

#include "sc_type.hpp"

/**
 * Base class for all objects that has meta data.
 * If you override it, then call any constructor of ScObject
 * in your custom constructors
 */
class ScObject
{
public:
  _SC_EXTERN ScObject & operator=(ScObject const & other) = default;

  _SC_EXTERN virtual ~ScObject() = default;

  _SC_EXTERN virtual std::string GetName()
  {
    return Demangle(typeid(*this).name());
  }

  _SC_EXTERN virtual sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) = 0;
  _SC_EXTERN virtual sc_result Shutdown(ScMemoryContext * ctx) = 0;

private:
  std::string Demangle(std::string const & mangled_name)
  {
    std::string demangled;
    size_t i = 0;

    if (mangled_name[0] == 'N')
      ++i;

    while (i < mangled_name.size() && std::isdigit(mangled_name[i]))
    {
      // Read the length of the next part
      size_t len = 0;
      while (i < mangled_name.size() && std::isdigit(mangled_name[i]))
      {
        len = len * 10 + (mangled_name[i] - '0');
        ++i;
      }

      // Extract the part of the name
      if (i + len <= mangled_name.size())
      {
        if (!demangled.empty())
        {
          demangled += "::";
        }
        demangled += mangled_name.substr(i, len);
        i += len;
      }
      else
      {
        // If the length is invalid, return the original mangled name
        return mangled_name;
      }
    }

    return demangled;
  }
};
