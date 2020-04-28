/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/sc_debug.hpp"

#define PARSE_ERROR(source, line, description) \
{ \
  SC_THROW_EXCEPTION(utils::ExceptionParseError, "Error: '" << description << "' in " << source << " at line " << line); \
}
