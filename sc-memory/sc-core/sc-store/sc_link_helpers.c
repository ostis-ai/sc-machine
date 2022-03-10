/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_link_helpers.h"
#include "sc_element.h"

#include <memory.h>
#include <glib.h>


sc_bool sc_link_get_content(const sc_stream *stream, sc_char **content, sc_uint16 *size)
{
  sc_stream_seek(stream, SC_STREAM_SEEK_SET, 0);

  sc_uint32 length = 0;
  if (sc_stream_get_length(stream, &length) == SC_RESULT_ERROR)
    return SC_FALSE;

  if (length == 0)
    return SC_TRUE;

  sc_char *buffer = g_malloc0(sizeof(sc_char) * length + 2);
  sc_uint32 data_read;
  if (sc_stream_read_data(stream, buffer, length, &data_read) == SC_RESULT_ERROR)
  {
    return SC_FALSE;
  }

  if (length != data_read)
    return SC_FALSE;

  *content = g_malloc0(sizeof(sc_char) * length);
  memcpy(*content, buffer, length);
  (*content)[length] = '\0';

  *size = length;

  return SC_TRUE;
}
