/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
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
sc_stream* sc_stream_memory_new(sc_char *buffer, sc_uint buffer_size, sc_uint8 flags, sc_bool data_owner);

#endif // SC_STREAM_MEMORY_H
