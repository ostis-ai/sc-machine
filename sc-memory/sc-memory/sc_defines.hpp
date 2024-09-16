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

// It will be used, when sc-machine will support windows. It is typedef for __declspec(dllexport), that is a
// Microsoft-specific storage-class specifier used in C and C++ programming to indicate that a function, data, or object
// is to be exported from a Dynamic NodeLink Library (DLL). When you declare a function or variable with
// __declspec(dllexport), it adds an export directive to the object file, which means that the symbol can be linked to
// from outside the DLL. This eliminates the need for a module-definition (.DEF) file for specifying exported functions.
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
