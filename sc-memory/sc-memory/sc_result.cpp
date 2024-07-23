/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_result.hpp"

ScResult::ScResult(sc_result code)
  : m_code(code)
{
}

ScResult::operator sc_result()
{
  return m_code;
}

void ScResult::Initialize(ScMemoryContext *, ScAddr const &) {}

void ScResult::Shutdown(ScMemoryContext *) {}
