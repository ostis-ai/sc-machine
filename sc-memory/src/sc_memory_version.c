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

#include "sc_memory_version.h"

#include <glib.h>

const sc_version SC_VERSION = {0, 2, 0, "rc1"};


sc_int32 sc_version_compare(const sc_version *a, const sc_version *b)
{
    g_assert(a && b);

    if (a->major < b->major) return -1;
    if (a->major > b->major) return 1;

    if (a->minor < b->minor) return -1;
    if (a->minor > b->minor) return 1;

    if (a->patch < b->patch) return -1;
    if (a->patch > b->patch) return 1;

    return 0;
}

char* sc_version_string_new(const sc_version *v)
{
    if (v->suffix)
        return g_strdup_printf("%u.%u.%u %s", v->major, v->minor, v->patch, v->suffix);

    return g_strdup_printf("%u.%u.%u", v->major, v->minor, v->patch);
}

void sc_version_string_free(char *str)
{
    g_assert(str != 0);
    g_free(str);
}

sc_uint32 sc_version_to_int(sc_version const * version)
{
    g_assert(version);
    return (version->major << 16) | (version->minor << 8) | version->patch;
}

void sc_version_from_int(sc_uint32 value, sc_version * version)
{
    g_assert(version);
    version->patch = value & 0xff;
    version->minor = value >> 8 & 0xff;
    version->major = value >> 16 & 0xff;
}
