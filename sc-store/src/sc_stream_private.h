#ifndef _sc_stream_private_h_
#define _sc_stream_private_h_

#include "sc_stream.h"

/*! Pointer type to stream read function. This function read data into specified \i buffer with
 * fixed \i length and return number of bytes, that has been read.
 */
typedef sc_result (*fStreamRead)(sc_stream *stream, sc_char *data, sc_uint32 length, sc_uint32 *bytes_read);

/*! Pointer type to stream write function. This function write data into pointer from specifed \i buffer
 * with fixed \i length and return number of bytes, that has been written.
 */
typedef sc_result (*fStreamWrite)(sc_stream *stream, sc_char *data, sc_uint32 length, sc_uint32 *bytes_written);

/*! Pointer type to stream seek function. This function to seek data in stream from \i origin with \i offset.
 */
typedef sc_result (*fStreamSeek)(sc_stream *stream, sc_stream_seek origin, sc_uint32 offset);

/*! Pointer type to stream tell function. This function return \i position in file
 */
typedef sc_result (*fStreamTell)(sc_stream *stream, sc_uint32 *position);

/*! Structure to store stream information
 */
struct _sc_stream
{
    //! Pointer to resource handle (file handle, network handle, ...)
    void *handle;
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
};


#endif
