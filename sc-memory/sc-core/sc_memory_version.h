/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_version_h_
#define _sc_memory_version_h_

#include "sc-store/sc_types.h"

typedef struct _sc_version
{
  sc_uint8 major;
  sc_uint8 minor;
  sc_uint8 patch;
  sc_char const * suffix;
} sc_version;

/*! Function compares to versions @p a and @p b.
 * @returns If version @p a less then version @p b, then returns -1.
 * If version @p a more then version @p b, then returns 1.
 * If version @p a and @p b equal, then returns 0.
 * @note This function ignores suffixes
 */
_SC_EXTERN sc_int32 sc_version_compare(sc_version const * a, sc_version const * b);

/*! Returns newly-allocated string that represents version.
 * The returned string should be freed with sc_version_string_free when no longer needed.
 */
_SC_EXTERN char * sc_version_string_new(sc_version const * v);

/*! Frees string that represents sc-version
 */
_SC_EXTERN void sc_version_string_free(sc_char * str);

_SC_EXTERN sc_uint32 sc_version_to_int(sc_version const * version);
_SC_EXTERN void sc_version_from_int(sc_uint32 value, sc_version * version);

#endif
