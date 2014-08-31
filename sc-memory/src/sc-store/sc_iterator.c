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

#include "sc_iterator.h"

#include <glib.h>

GSList *time_stamps_list = 0;
sc_uint32 time_stamps_count = 0; // store cached value to prevent sequence length calculation

#if SC_INTERNAL_THREADS_SUPPORT
    GStaticMutex time_stamp_list_mutex = G_STATIC_MUTEX_INIT;
    #define TIMESTAMP_LIST_LOCK g_static_mutex_lock(&time_stamp_list_mutex);
    #define TIMESTAMP_LIST_UNLOCK g_static_mutex_unlock(&time_stamp_list_mutex);
#else
    #define TIMESTAMP_LIST_LOCK
    #define TIMESTAMP_LIST_UNLOCK
#endif

gint _list_compare(gconstpointer a, gconstpointer b)
{
    sc_uint32 a_time = GPOINTER_TO_UINT(a);
    sc_uint32 b_time = GPOINTER_TO_UINT(b);

    if (a_time == b_time) return 0;
    if (a_time < b_time) return -1;

    return 1;
}


