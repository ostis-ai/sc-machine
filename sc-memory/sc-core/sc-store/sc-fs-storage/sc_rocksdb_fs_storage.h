/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_rocksdb_fs_storage_h_
#define _sc_rocksdb_fs_storage_h_

#include "../sc_types.h"
#include "../sc_stream.h"

sc_result sc_rocksdb_fs_storage_initialize(const sc_char * repo_path);
void sc_rocksdb_fs_storage_shutdown();

sc_char * sc_rocksdb_fs_storage_read_stream_new(const sc_check_sum * check_sum);
sc_result sc_rocksdb_fs_storage_write_stream(const sc_check_sum * check_sum, const sc_char * sc_string);

sc_result sc_rocksdb_fs_storage_addr_ref_append(sc_addr addr, const sc_check_sum * check_sum);
sc_result sc_rocksdb_fs_storage_addr_ref_remove(sc_addr addr, const sc_check_sum * check_sum);

sc_result sc_rocksdb_fs_storage_find(const sc_check_sum * check_sum, sc_addr ** result, sc_uint32 * result_count);

sc_result sc_rocksdb_fs_storage_clear();
sc_result sc_rocksdb_fs_storage_save();
sc_result sc_rocksdb_fs_storage_clean_state();

#endif  // _sc_rocksdb_fs_storage_h_
