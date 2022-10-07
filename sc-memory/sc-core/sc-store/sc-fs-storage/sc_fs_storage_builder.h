/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_fs_storage.h"

#ifdef SC_DICTIONARY_FS_STORAGE
#  include "sc_dictionary_fs_storage.h"
#elif SC_ROCKSDB_FS_STORAGE
#  include "sc_rocksdb_fs_storage.h"
#endif

#include "../sc-base/sc_allocator.h"

sc_fs_storage * sc_fs_storage_build()
{
  sc_fs_storage * storage = sc_mem_new(sc_fs_storage, 1);
#ifdef SC_DICTIONARY_FS_STORAGE
  storage->initialize = sc_dictionary_fs_storage_initialize;
  storage->shutdown = sc_dictionary_fs_storage_shutdown;
  storage->fill = sc_dictionary_fs_storage_fill;
  storage->save = sc_dictionary_fs_storage_save;
  storage->clear = null_ptr;
  storage->append_sc_link_content = sc_dictionary_fs_storage_append_sc_link_content;
  storage->get_sc_links_by_content = sc_dictionary_fs_storage_get_sc_links_by_content;
  storage->get_sc_links_by_content_substring = sc_dictionary_fs_storage_get_sc_links_by_content_substr;
  storage->get_sc_links_contents_by_content_substring =
      sc_dictionary_fs_storage_get_sc_links_contents_by_content_substr;
  storage->get_sc_link_content_ext = sc_dictionary_fs_storage_get_sc_link_content_ext;
  storage->remove_sc_link_content = sc_dictionary_fs_storage_remove_sc_link_content;
#elif SC_ROCKSDB_FS_STORAGE
  storage->initialize = sc_rocksdb_fs_storage_initialize;
  storage->shutdown = sc_rocksdb_fs_storage_shutdown;
  storage->fill = null_ptr;
  storage->save = sc_rocksdb_fs_storage_save;
  storage->clear = sc_rocksdb_fs_storage_clear;
  storage->append_sc_link_content = sc_rocksdb_fs_storage_append_sc_link_content;
  storage->get_sc_links_by_content = sc_rocksdb_fs_storage_get_sc_links_by_content;
  storage->get_sc_links_by_content_substring = sc_rocksdb_fs_storage_find_sc_links_by_content_substr;
  storage->get_sc_links_contents_by_content_substring = sc_rocksdb_fs_storage_find_sc_links_contents_by_content_substr;
  storage->get_sc_link_content_ext = sc_rocksdb_fs_storage_get_sc_link_content_ext;
  storage->remove_sc_link_content = sc_rocksdb_fs_storage_remove_sc_link_content;
#endif

  return storage;
}
