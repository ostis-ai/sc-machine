/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_io_h_
#define _sc_io_h_

#include <glib.h>

#include "sc-core/sc_types.h"

typedef GIOChannel sc_io_channel;

/// io statuses
#define SC_FS_IO_STATUS_NORMAL G_IO_STATUS_NORMAL

/// seek types
#define SC_FS_IO_SEEK_SET G_SEEK_SET

#define sc_io_new_channel(file_path, mode, errors) g_io_channel_new_file(file_path, mode, errors)

#define sc_io_new_append_channel(file_path, errors) sc_io_new_channel(file_path, "r+", errors)

#define sc_io_new_read_channel(file_path, errors) sc_io_new_channel(file_path, "r", errors)

#define sc_io_new_write_channel(file_path, errors) sc_io_new_channel(file_path, "w+", errors)

#define sc_io_channel_set_encoding(channel, encoding, errors) g_io_channel_set_encoding(channel, encoding, errors)

#define sc_io_channel_flush(channel, errors) g_io_channel_flush(channel, errors)

#define sc_io_channel_shutdown(channel, flush, errors) \
  g_io_channel_shutdown(channel, flush, errors); \
  g_io_channel_unref(channel)

#define sc_io_channel_write_chars(channel, chars, count, written_bytes, errors) \
  g_io_channel_write_chars(channel, (sc_char *)chars, count, (gsize *)written_bytes, errors)

#define sc_io_channel_read_chars(channel, chars, count, read_bytes, errors) \
  g_io_channel_read_chars(channel, chars, count, (gsize *)read_bytes, errors)

#define sc_io_channel_seek(channel, offset, type, errors) g_io_channel_seek_position(channel, offset, type, errors)

#endif
