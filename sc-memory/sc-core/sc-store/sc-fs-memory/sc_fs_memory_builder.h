/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifdef SC_DICTIONARY_FS_MEMORY
#  include "sc_dictionary_fs_memory.h"
#endif

#include "sc_fs_memory.h"

#include "../sc-base/sc_allocator.h"

sc_fs_memory_manager * sc_fs_memory_build()
{
  sc_fs_memory_manager * manager = sc_mem_new(sc_fs_memory_manager, 1);
#ifdef SC_DICTIONARY_FS_MEMORY
  manager->initialize = sc_dictionary_fs_memory_initialize_ext;
  manager->shutdown = sc_dictionary_fs_memory_shutdown;
  manager->load = sc_dictionary_fs_memory_load;
  manager->save = sc_dictionary_fs_memory_save;
  manager->link_string = sc_dictionary_fs_memory_link_string;
  manager->get_link_hashes_by_string = sc_dictionary_fs_memory_get_link_hashes_by_string;
  manager->get_link_hashes_by_substring = sc_dictionary_fs_memory_get_link_hashes_by_substring_ext;
  manager->get_strings_by_substring = sc_dictionary_fs_memory_get_strings_by_substring_ext;
  manager->get_string_by_link_hash = sc_dictionary_fs_memory_get_string_by_link_hash;
  manager->unlink_string = sc_dictionary_fs_memory_unlink_string;
#endif

  return manager;
}
