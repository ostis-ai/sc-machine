/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_hash_table_h_
#define _sc_hash_table_h_

#include <glib.h>

typedef GHashTable sc_hash_table;

#define sc_hash_table_init(hash_func, key_equal_func, key_destroy_func, value_destroy_func) \
  g_hash_table_new_full(hash_func, key_equal_func, key_destroy_func, value_destroy_func)

#define sc_hash_table_destroy(table) g_hash_table_destroy(table)

#define sc_hash_table_size(table) g_hash_table_size(table)

#define sc_hash_table_insert(table, key, value) g_hash_table_insert(table, key, value)

#define sc_hash_table_get(table, key) g_hash_table_lookup(table, key)

#define sc_hash_table_remove(table, key) g_hash_table_remove(table, key)

#define sc_hash_table_default_hash_func g_direct_hash

#define sc_hash_table_default_equal_func g_direct_equal

typedef GSList sc_hash_table_list;

#define sc_hash_table_list_destroy(list) g_slist_free(list)

#define sc_hash_table_list_append(list, value) g_slist_append(list, value)

#define sc_hash_table_list_remove(list, value) g_slist_remove(list, value)

#define sc_hash_table_list_remove_sublist(list, sublist) g_slist_delete_link(list, sublist)

typedef GHashTableIter sc_hash_table_iterator;

#define sc_hash_table_iterator_init(iterator, hash_table) g_hash_table_iter_init(iterator, hash_table)

#define sc_hash_table_iterator_next(iterator, key, value) g_hash_table_iter_next(iterator, key, value)

#endif
