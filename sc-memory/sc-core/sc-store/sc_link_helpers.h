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
sc_bool sc_link_calculate_checksum(const sc_char * sc_string, sc_uint32 size, sc_check_sum ** check_sum);
#endif

sc_bool sc_link_get_content(const sc_stream * stream, sc_char ** content, sc_uint32 * size);

#endif
