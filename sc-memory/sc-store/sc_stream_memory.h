/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef SC_STREAM_MEMORY_H
#define SC_STREAM_MEMORY_H

#include "sc_stream.h"

/*! Create memory data stream
 * @param buffer Pointer to memory buffer with data
 * @param buffer_size Size of data in buffer
 * @param flags Data stream flags
 * @param data_owner Flag that point to take ownership om data buffer by stream. Another words,
 * it means, that data will be free with stream.
 * @remarks The returned stream pointer should be freed with sc_stream_free function, when done using it.
 * @return Returns stream pointer if the stream was successfully created, or NULL if an error occurred
 */
_SC_EXTERN sc_stream* sc_stream_memory_new(const sc_char *buffer, sc_uint buffer_size, sc_uint8 flags, sc_bool data_owner);

#endif // SC_STREAM_MEMORY_H
