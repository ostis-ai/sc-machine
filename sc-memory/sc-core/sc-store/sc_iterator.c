/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#if SC_INTERNAL_THREADS_SUPPORT

#  include <glib.h>

GSList * time_stamps_list = 0;
sc_uint32 time_stamps_count = 0;  // store cached value to prevent sequence length calculation

GStaticMutex time_stamp_list_mutex = G_STATIC_MUTEX_INIT;
#  define TIMESTAMP_LIST_LOCK g_static_mutex_lock(&time_stamp_list_mutex);
#  define TIMESTAMP_LIST_UNLOCK g_static_mutex_unlock(&time_stamp_list_mutex);
#else
#  define TIMESTAMP_LIST_LOCK
#  define TIMESTAMP_LIST_UNLOCK
#endif
