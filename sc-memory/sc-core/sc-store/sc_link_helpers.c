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

  *hash_string = g_new0(sc_char, strlen(result) + 1);
  memcpy(*hash_string, &(result[0]), strlen(result));

  g_checksum_free(checksum);

  sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

  return SC_TRUE;
}

sc_bool sc_link_get_content(const sc_stream *stream, sc_char **content, sc_uint32 *size)
{
  sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

  sc_uint32 length = 0;
  if (sc_stream_get_length(stream, &length) == SC_RESULT_ERROR)
    return SC_FALSE;

  if (length == 0)
    return SC_TRUE;

  *content = g_new0(sc_char, length + 1);
  if (sc_stream_read_data(stream, *content, length, (sc_uint32 *)size) == SC_RESULT_ERROR)
    return SC_FALSE;

  if (length != *size)
  {
    *content = null_ptr;
    *size = 0;
    return SC_FALSE;
  }

  return SC_TRUE;
}
