/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_fs_memory_header.h"

#include "sc_dictionary_fs_memory_private.h"

sc_fs_memory_status sc_fs_memory_header_read(sc_io_channel * channel, sc_fs_memory_header * header)
{
  sc_uint64 read_bytes = 0;
  sc_uint32 header_size = 0;
  if (sc_io_channel_read_chars(channel, (sc_char *)&header_size, sizeof(header_size), &read_bytes, null_ptr) !=
          SC_FS_IO_STATUS_NORMAL ||
      read_bytes != sizeof(header_size))
  {
    sc_fs_memory_error("Error while attribute `header_size` reading");
    return SC_FS_MEMORY_READ_ERROR;
  }

  if (header_size != sizeof(sc_fs_memory_header))
  {
    sc_fs_memory_error("Invalid header size %d != %lu", header_size, sizeof(sc_fs_memory_header));
    return SC_FS_MEMORY_READ_ERROR;
  }

  if (sc_io_channel_read_chars(channel, (sc_char *)header, sizeof(sc_fs_memory_header), &read_bytes, null_ptr) !=
          SC_FS_IO_STATUS_NORMAL ||
      read_bytes != sizeof(sc_fs_memory_header))
  {
    sc_fs_memory_error("Error while attribute `header` reading");
    return SC_FS_MEMORY_READ_ERROR;
  }

  return SC_FS_MEMORY_OK;
}

sc_fs_memory_status sc_fs_memory_header_write(sc_io_channel * channel, sc_fs_memory_header const header)
{
  sc_uint64 write_bytes = 0;
  sc_uint32 header_size = sizeof(sc_fs_memory_header);
  if (sc_io_channel_write_chars(channel, &header_size, sizeof(header_size), &write_bytes, null_ptr) !=
          SC_FS_IO_STATUS_NORMAL ||
      write_bytes != sizeof(header_size))
  {
    sc_fs_memory_error("Error while attribute `header_size` writing");
    return SC_FS_MEMORY_WRITE_ERROR;
  }

  if (sc_io_channel_write_chars(channel, &header, sizeof(sc_fs_memory_header), &write_bytes, null_ptr) !=
          SC_FS_IO_STATUS_NORMAL ||
      write_bytes != sizeof(sc_fs_memory_header))
  {
    sc_fs_memory_error("Error while attribute `header` writing");
    return SC_FS_MEMORY_WRITE_ERROR;
  }

  return SC_FS_MEMORY_OK;
}
