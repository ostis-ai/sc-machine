/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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

void sc_iterator_add_used_timestamp(sc_uint32 time_stamp)
{
    TIMESTAMP_LIST_LOCK;
    time_stamps_list = g_slist_insert_sorted(time_stamps_list, GUINT_TO_POINTER(time_stamp), _list_compare);
    time_stamps_count++;
    TIMESTAMP_LIST_UNLOCK
}

void sc_iterator_remove_used_timestamp(sc_uint32 time_stamp)
{
    TIMESTAMP_LIST_LOCK;
    time_stamps_list = g_slist_remove(time_stamps_list, GUINT_TO_POINTER(time_stamp));
    time_stamps_count--;
    TIMESTAMP_LIST_UNLOCK;
}

sc_uint32 sc_iterator_get_oldest_timestamp()
{
    sc_uint32 res = 0;
    if (time_stamps_count > 0)
    {
        TIMESTAMP_LIST_LOCK;
        res = GPOINTER_TO_UINT(time_stamps_list->data);
        TIMESTAMP_LIST_UNLOCK;
    }

    return res;
}

sc_bool sc_iterator_has_any_timestamp()
{
    sc_bool res = SC_FALSE;

    TIMESTAMP_LIST_LOCK;
    res = time_stamps_list != 0;
    TIMESTAMP_LIST_UNLOCK;

    return res;
}

