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

  static _SC_EXTERN std::string GetName();

  _SC_EXTERN virtual sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) = 0;
  _SC_EXTERN virtual sc_result Shutdown(ScMemoryContext * ctx) = 0;
};
