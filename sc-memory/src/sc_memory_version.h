/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/
#ifndef _sc_memory_version_h_
#define _sc_memory_version_h_

#include "sc-store/sc_types.h"

struct _sc_version
{
    sc_uint8 major;
    sc_uint8 minor;
    sc_uint8 patch;
    const char *suffix;
};

typedef struct _sc_version sc_version;


extern const sc_version SC_VERSION;

/*! Function compares to versions @p a and @p b.
 * @returns If version @p a less then version @p b, then returns -1.
 * If version @p a more then version @p b, then returns 1.
 * If version @p a and @p b equal, then returns 0.
 * @note This function ignores suffixes
 */
sc_int32 sc_version_compare(const sc_version *a, const sc_version *b);

/*! Returns newvly-alocated string that represents version.
 * The returned string should be freed with sc_version_string_free when no longer needed.
 */
char* sc_version_string_new(const sc_version *v);

/*! Frees string that represents sc-version
 */
void sc_version_string_free(char *str);

sc_uint32 sc_version_to_int(sc_version const * version);
void sc_version_from_int(sc_uint32 value, sc_version * version);

#endif
