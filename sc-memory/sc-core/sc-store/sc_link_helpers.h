/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_link_helpers_h_
#define _sc_link_helpers_h_

#include "sc_types.h"
#include "sc_stream.h"

#ifdef SC_ROCKSDB_FS_STORAGE
/*! Caclulates checksum for data in stream
 * @param stream Pointer to data stream for checksum calculation
 * @param check_sum Pointer to stucture, that contains calculated checksum
 * @return If checksum calculated, then return SC_TRUE; otherwise return SC_FALSE
 */
sc_bool sc_link_calculate_checksum(const sc_char * sc_string, sc_uint32 size, sc_check_sum ** check_sum);
#endif

#endif
