/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_fs_memory_header_h_
#define _sc_fs_memory_header_h_

#include "../sc_types.h"
#include "sc_fs_memory_status.h"
#include "sc_io.h"

#define DEFAULT_CHECKSUM_SIZE 64

typedef struct _sc_fs_memory_header
{
  sc_uint32 version;
  sc_uint16 size;  // deprecated in 0.8.0
  sc_uint64 timestamp;
  sc_uint8 checksum[DEFAULT_CHECKSUM_SIZE];
} sc_fs_memory_header;

sc_fs_memory_status sc_fs_memory_header_read(sc_io_channel * channel, sc_fs_memory_header * header);

sc_fs_memory_status sc_fs_memory_header_write(sc_io_channel * channel, sc_fs_memory_header header);

#endif
