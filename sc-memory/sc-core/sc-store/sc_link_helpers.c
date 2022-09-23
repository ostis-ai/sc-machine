/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_link_helpers.h"

#include "sc-base/sc_allocator.h"

#ifdef SC_ROCKSDB_FS_STORAGE

#  define SC_DEFAULT_CHECKSUM G_CHECKSUM_SHA256

sc_bool sc_link_calculate_checksum(const sc_char * sc_string, sc_uint32 size, sc_check_sum ** check_sum)
{
  *check_sum = sc_mem_new(sc_check_sum, 1);
  (*check_sum)->len = (sc_uint8)g_checksum_type_get_length(SC_DEFAULT_CHECKSUM);

  sc_char * result = g_compute_checksum_for_string(SC_DEFAULT_CHECKSUM, sc_string, size);
  sc_mem_cpy((*check_sum)->data, result, (*check_sum)->len);
  sc_mem_free(result);

  return SC_TRUE;
}
#endif
