/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_stream_private_h_
#define _sc_stream_private_h_

#include "sc_stream.h"

/*! Pointer to stream read function. This function read data into specified \i buffer with
 * fixed \i length and return number of bytes, that has been read.
 */
typedef sc_result (*fStreamRead)(const sc_stream *stream, sc_char *data, sc_uint32 length, sc_uint32 *bytes_read);

/*! Pointer to stream write function. This function write data into pointer from specifed \i buffer
 * with fixed \i length and return number of bytes, that has been written
 */
typedef sc_result (*fStreamWrite)(const sc_stream *stream, sc_char *data, sc_uint32 length, sc_uint32 *bytes_written);

/*! Pointer to stream seek function. This function to seek data in stream from \i origin with \i offset.
 */
typedef sc_result (*fStreamSeek)(const sc_stream *stream, sc_stream_seek_origin origin, sc_uint32 offset);

/*! Pointer to stream tell function. This function return \i position in file
 */
typedef sc_result (*fStreamTell)(const sc_stream *stream, sc_uint32 *position);

/*! Pointer to stream eof function. This function return SC_TRUE, if current position in stream is at the end
 */
typedef sc_bool (*fStreamEof)(const sc_stream *stream);

/*! Pointer to stream handler free function. This function destroys stream handler.
 */
typedef sc_result (*fStreamFreeHandler)(const sc_stream *stream);

/*! Structure to store stream information
 */
struct _sc_stream
{
    //! Pointer to resource handle (file handle, network handle, ...)
    void *handler;
    //! Access flags (read, write, append, ...)
    sc_uint8 flags;
    //! Pointer to a function that reads data from stream
    fStreamRead read_func;
    //! Pointer to a function that writes data into stream
    fStreamWrite write_func;
    //! Pointer to a function that sets the position indicator associated with the stream to a new position
    fStreamSeek seek_func;
    //! Pointer to a function that returns the current value of the position indicator of the stream
    fStreamTell tell_func;
    //! Pointer to function for handler freeing
    fStreamFreeHandler free_func;
    //! Pointer to function to check if stream indicates to the end position
    fStreamEof eof_func;
};


#endif
