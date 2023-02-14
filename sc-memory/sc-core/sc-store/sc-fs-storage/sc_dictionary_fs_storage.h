/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_fs_storage_h_
#define _sc_dictionary_fs_storage_h_

#include "../sc_types.h"
#include "../sc_defines.h"
#include "../sc_stream.h"

#include "../sc-container/sc-dictionary/sc_dictionary.h"
#include "../sc-container/sc-list/sc_list.h"

typedef struct _sc_dictionary_fs_storage
{
  sc_char * path;
  
  sc_dictionary * terms_offsets_dictionary;
  sc_char * terms_offsets_path;

  sc_dictionary * string_offsets_link_hashes_dictionary;
  sc_char * string_offsets_link_hashes_path;

  sc_dictionary * link_hashes_string_offsets_dictionary;
  sc_char * link_hashes_string_offsets_path;

  sc_char * terms_path;
  sc_uint64 terms_size;

  sc_char * strings_path;
  sc_uint64 strings_size;
} sc_dictionary_fs_storage;

typedef enum _sc_dictionary_fs_storage_status
{
  SC_FS_STORAGE_OK,

  SC_FS_STORAGE_NO,
  SC_FS_STORAGE_WRONG_PATH,
  SC_FS_STORAGE_WRITE_ERROR,
  SC_FS_STORAGE_READ_ERROR
} sc_fs_storage_status;

sc_fs_storage_status sc_dictionary_fs_storage_initialize(sc_dictionary_fs_storage ** storage, sc_char const * path);

sc_fs_storage_status sc_dictionary_fs_storage_shutdown(sc_dictionary_fs_storage * storage);

sc_fs_storage_status sc_dictionary_fs_storage_add_strings(sc_dictionary_fs_storage * storage, sc_list const * strings_link_hashes);

sc_fs_storage_status sc_dictionary_fs_storage_get_strings(sc_dictionary_fs_storage * storage, sc_list const * terms, sc_list ** strings);

sc_fs_storage_status sc_dictionary_fs_storage_load(sc_dictionary_fs_storage * storage);

sc_fs_storage_status sc_dictionary_fs_storage_save(sc_dictionary_fs_storage * storage);

#endif //_sc_dictionary_fs_storage_h_
