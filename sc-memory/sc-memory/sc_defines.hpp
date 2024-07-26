/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

extern "C"
{
#include "sc-core/sc-store/sc_defines.h"
}

#define _SC_EXTERN

#define _SC_DELETED_FUNCTION

#define SC_UNUSED(__X) (void)__X

#if defined(__clang__) || defined(__GNUC__)
#  define SC_FALLTHROUGH [[fallthrough]]
#elif defined(_MSC_VER)
#  define SC_FALLTHROUGH __fallthrough
#else
#  define SC_FALLTHROUGH /* fall through */
#endif
