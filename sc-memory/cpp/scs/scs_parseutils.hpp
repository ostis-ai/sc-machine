/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <antlr3.hpp>
#include <stdint.h>

#define APPEND_ATTRS(__attrs, __edge) \
for (auto const & _a : __attrs) \
{ \
  ElementHandle const attrEdge = m_parser->ProcessConnector(_a.second ? "->" : "_->"); \
  m_parser->ProcessTriple(_a.first, attrEdge, __edge); \
}