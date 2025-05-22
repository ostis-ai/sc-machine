/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#ifndef _sc_dictionary_fs_memory_private_h_
#define _sc_dictionary_fs_memory_private_h_

#include "sc-core/sc_types.h"

#include "sc-core/sc-container/sc_list.h"
#include "sc-core/sc-container/sc_dictionary.h"

#include "sc-core/sc_memory_params.h"

#include "sc-store/sc-base/sc_monitor_private.h"
#include "sc-store/sc-base/sc_monitor_table_private.h"
#include "sc-store/sc-base/sc_message.h"
#include "sc_dictionary_fs_memory.h"
#include "sc_io.h"

#define SC_FS_EXT ".scdb"
#define INVALID_STRING_OFFSET LONG_MAX

#define SC_FS_MEMORY_PREFIX "[sc-fs-memory] "
#define sc_fs_memory_info(...) sc_message(SC_FS_MEMORY_PREFIX __VA_ARGS__)
#define sc_fs_memory_warning(...) sc_warning(SC_FS_MEMORY_PREFIX __VA_ARGS__)
#define sc_fs_memory_error(...) sc_critical(SC_FS_MEMORY_PREFIX __VA_ARGS__)






// Структура для буфера записи
typedef struct _sc_write_buffer
{
  sc_char * data;           // Массив байтов для хранения данных
  sc_uint64 size;           // Текущий размер данных в буфере
  sc_uint64 capacity;       // Максимальный размер буфера
  sc_monitor buffer_monitor; // Монитор для синхронизации доступа
} sc_write_buffer;


struct _sc_dictionary_fs_memory
{
 sc_char * path;  // path to all dictionary files
 sc_bool clear;

 sc_uint16 max_strings_channels;
 sc_uint32 max_strings_channel_size;
 sc_uint32 max_searchable_string_size;  // maximal size of strings that can be found by string/substring
 sc_char const * term_separators;
 sc_bool search_by_substring;

 void ** strings_channels;
 sc_monitor_table strings_channels_monitors_table;
 sc_uint64 last_string_offset;  // last offset of string in 'string_path`
 sc_monitor monitor;
 sc_monitor resolve_string_offset_monitor;

 sc_char * terms_string_offsets_path;              // path to dictionary file with terms and its strings offsets
 sc_dictionary * terms_string_offsets_dictionary;  // dictionary instance with terms and its strings offsets

 sc_char * string_offsets_link_hashes_path;  // path to dictionary file with strings offsets and its link hashes
 sc_dictionary *
     string_offsets_link_hashes_dictionary;  // dictionary instance with strings offsets and its link hashes
 sc_dictionary *
     link_hashes_string_offsets_dictionary;  // dictionary instance with link hashes and its strings offsets

 sc_write_buffer * write_buffer; // Буфер для группировки операций записи

};

sc_bool _sc_uchar_dictionary_initialize(sc_dictionary ** dictionary);

sc_bool _sc_number_dictionary_initialize(sc_dictionary ** dictionary);

void _sc_dictionary_fs_memory_node_clear(sc_dictionary_node * node);

void _sc_dictionary_fs_memory_link_node_clear(sc_dictionary_node * node);

void _sc_dictionary_fs_memory_string_node_clear(sc_dictionary_node * node);

sc_memory_params * _sc_dictionary_fs_memory_get_default_params(sc_char const * path, sc_bool clear);

sc_char * _sc_dictionary_fs_memory_get_first_term(sc_char const * string, sc_char const * term_separators);

sc_list * _sc_dictionary_fs_memory_get_string_terms(sc_char const * string, sc_char const * term_separators);




void _sc_dictionary_fs_memory_buffer_init(sc_write_buffer ** buffer, sc_uint64 capacity);
void _sc_dictionary_fs_memory_buffer_destroy(sc_write_buffer * buffer);
sc_bool _sc_dictionary_fs_memory_buffer_append(
    sc_write_buffer * buffer,
    sc_char const * data,
    sc_uint64 size);
sc_dictionary_fs_memory_status _sc_dictionary_fs_memory_buffer_flush(
    sc_dictionary_fs_memory * memory,
    sc_write_buffer * buffer,
    sc_io_channel * channel,
    sc_monitor * channel_monitor);

#endif