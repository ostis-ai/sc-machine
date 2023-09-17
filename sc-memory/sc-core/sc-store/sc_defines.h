/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_defines_h_
#define _sc_defines_h_

#include "sc_platform.h"

#ifdef SC_DEBUG
#  define SC_DEBUG_MODE 1
#else
#  define SC_DEBUG_MODE 0
#endif

#define MAX_PATH_LENGTH 1024

#if defined(SC_MEMORY_SELF_BUILD)
#  if defined(SC_PLATFORM_WIN)
#    define _SC_EXTERN __declspec(dllexport)
#  else
#    define _SC_EXTERN
#  endif
#else
#  if defined(SC_PLATFORM_WIN)
#    define _SC_EXTERN __declspec(dllimport)
#  else
#    define _SC_EXTERN
#  endif
#endif

#if defined(SC_PLATFORM_WIN)
#  if defined(__cplusplus)
#    define _SC_EXT_EXTERN extern "C" __declspec(dllexport)
#  else
#    define _SC_EXT_EXTERN __declspec(dllexport)
#  endif
#else
#  define _SC_EXT_EXTERN
#endif

// -------------- Deprecation ---------------
#if (SC_COMPILER == SC_COMPILER_MSVC)

//__declspec(deprecated(__Message))// "Update you code to newest API version " #__Version " or later."))
#  define _SC_DEPRECATED_IMPL(__Version, __Message) \
    __declspec(deprecated(__Message " Update you code to newest API version " #__Version " or later."))

#  define PRAGMA_DISABLE_DEPRECATION_WARNINGS \
    __pragma(warning(push)) __pragma(warning(disable : 4995)) __pragma(warning(disable : 4996))

#  define PRAGMA_ENABLE_DEPRECATION_WARNINGS __pragma(warning(pop))

#elif (SC_COMPILER == SC_COMPILER_CLANG)

#  define _SC_DEPRECATED_IMPL(__Version, __Message) \
    __attribute__((deprecated(__Message "Update you code to newest API version " #__Version " or later.")))

#elif (SC_COMPILER == SC_COMPILER_GNU)
#  define _SC_DEPRECATED_IMPL(__Version, __Message)
#else
#  define _SC_DEPRECATED_IMPL(__Version, __Message)
#endif

#define SC_DEPRECATED(__Version, __Message) _SC_DEPRECATED_IMPL(__Version, __Message)

#endif  // _sc_defines_h_
