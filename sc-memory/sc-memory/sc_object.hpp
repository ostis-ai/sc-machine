/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_defines.hpp"
#include "sc_addr.hpp"
#include "sc_memory.hpp"

/**
 * Base class for all objects that has meta data.
 * If you override it, then call any constructor of ScObject
 * in your custom constructors
 */
class _SC_EXTERN ScObject
{
protected:
  explicit ScObject();
  virtual ~ScObject();

  ScObject(ScObject const & other) = delete;
  ScObject & operator=(ScObject const & other);

public:
  /// TODO: Need mechanism to call that function automatically after object construction
  bool Init();

private:
  /** This method override generates by code generator, and initialize all
   *  meta data for this object, insert created object in output structure.
   *  It calls from ScObject constructors
   */
  virtual bool _InitInternal(ScAddr const & outputStructure = ScAddr::Empty) = 0;

  virtual bool _InitInternal(ScMemoryContext & context, ScAddr const & outputStructure = ScAddr::Empty) = 0;

private:
  bool m_isInitialized : 1;
  bool m_initResult : 1;
};
