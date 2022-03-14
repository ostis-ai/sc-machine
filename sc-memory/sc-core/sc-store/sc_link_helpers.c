/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_link_helpers.h"
#include "sc_element.h"

#include <memory.h>
#include <glib.h>

#define SC_DEFAULT_CHECKSUM G_CHECKSUM_SHA256
#define SC_CHECKSUM_LEN 32
#define SC_DEFAULT_BUFFER 1024

sc_bool sc_link_calculate_checksum(const sc_stream *stream, sc_char **hash_string)
{
  g_assert(stream != null_ptr);

  GChecksum *checksum = g_checksum_new(SC_DEFAULT_CHECKSUM);
  g_checksum_reset(checksum);

  sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

  sc_char buffer[SC_DEFAULT_BUFFER];
  sc_uint32 data_read;
  while (sc_stream_eof(stream) == SC_FALSE)
  {
    if (sc_stream_read_data(stream, buffer, SC_DEFAULT_BUFFER, &data_read) == SC_RESULT_ERROR)
    {
      g_checksum_free(checksum);
      return SC_FALSE;
    }
    g_checksum_update(checksum, (guchar*)buffer, data_read);
  }

  const sc_char *result = null_ptr;
  result = g_checksum_get_string(checksum);

  *hash_string = g_new0(sc_char, strlen(result));
  memcpy(*hash_string, result, strlen(result));

  g_checksum_free(checksum);

  sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

  return SC_TRUE;
}

sc_bool sc_link_get_content(const sc_stream *stream, sc_char **content, sc_uint16 *size)
{
  sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

  sc_uint32 length = 0;
  if (sc_stream_get_length(stream, &length) == SC_RESULT_ERROR)
    return SC_FALSE;

  if (length == 0)
    return SC_TRUE;

  sc_char *buffer = g_new0(sc_char, length);
  sc_uint32 data_read;
  if (sc_stream_read_data(stream, buffer, length, &data_read) == SC_RESULT_ERROR)
  {
    return SC_FALSE;
  }

  if (length != data_read)
    return SC_FALSE;

  *content = buffer;
  *size = length;

  return SC_TRUE;
}
