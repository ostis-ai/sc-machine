/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <typeinfo>
#include <string>

#include "sc_addr.hpp"

/**
 * Base class for all objects that has meta data.
 * If you override it, then call any constructor of ScObject in your custom constructors.
 */
class ScObject
{
public:
  _SC_EXTERN ScObject & operator=(ScObject const & other);

  _SC_EXTERN virtual ~ScObject();

  _SC_EXTERN std::string GetName();

  template <class Class>
  static _SC_EXTERN std::string GetName()
  {
    return Demangle(typeid(Class).name());
  }

  _SC_EXTERN virtual void Initialize(class ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) = 0;
  _SC_EXTERN virtual void Shutdown(class ScMemoryContext * ctx) = 0;

private:
  std::string m_name;

  static _SC_EXTERN std::string Demangle(std::string const & mangled_name);
};
