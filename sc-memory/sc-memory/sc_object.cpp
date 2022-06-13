/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_object.hpp"

ScObject::ScObject()
  : m_isInitialized(false)
  , m_initResult(false)
{
}

ScObject::~ScObject() = default;

ScObject::ScObject(ScObject const & other)
{
}

ScObject & ScObject::operator=(ScObject const & other)
{
  return *this;
}

bool ScObject::Init()
{
  if (!m_isInitialized)
  {
    m_isInitialized = true;
    m_initResult = _InitInternal();
  }

  return m_initResult;
}
