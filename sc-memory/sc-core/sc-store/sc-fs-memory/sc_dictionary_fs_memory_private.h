/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_fs_memory_private_h_
#define _sc_dictionary_fs_memory_private_h_

#include "../sc-base/sc_message.h"
#include "../sc_types.h"

#include "../sc-container/sc-list/sc_list.h"
#include "../sc-container/sc-dictionary/sc_dictionary.h"

#include "../../sc_memory_params.h"

#define SC_FS_EXT ".scdb"
#define INVALID_STRING_OFFSET LONG_MAX

#define SC_FS_MEMORY_PREFIX "[sc-fs-memory] "
#define sc_fs_memory_info(...) sc_message(SC_FS_MEMORY_PREFIX __VA_ARGS__)
#define sc_fs_memory_error(...) sc_critical(SC_FS_MEMORY_PREFIX __VA_ARGS__)

sc_bool _sc_uchar_dictionary_initialize(sc_dictionary ** dictionary);

sc_bool _sc_number_dictionary_initialize(sc_dictionary ** dictionary);

void _sc_dictionary_fs_memory_node_clear(sc_dictionary_node * node);

void _sc_dictionary_fs_memory_link_node_clear(sc_dictionary_node * node);

void _sc_dictionary_fs_memory_string_node_clear(sc_dictionary_node * node);

sc_memory_params * _sc_dictionary_fs_memory_get_default_params(sc_char const * path, sc_bool clear);

sc_char * _sc_dictionary_fs_memory_get_first_term(sc_char const * string, sc_char const * term_separators);

sc_list * _sc_dictionary_fs_memory_get_string_terms(sc_char const * string, sc_char const * term_separators);

#endif
