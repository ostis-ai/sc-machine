#ifndef _sc_stream_h_
#define _sc_stream_h_

#include "sc_types.h"

//! Type of data stream
typedef struct _sc_stream sc_stream;

enum _sc_stream_flags
{
    //! Data can be read from stream
    SC_STREAM_READ = 1,
    //! Data can be written into stream
    SC_STREAM_WRITE = 2,
    //! Stream opened for appending (compatible just with SC_STREAM_WRITE flag)
    SC_STREAM_APPEND = 4,
    //! Seek support (SEEK_SET, SEEK_CUR, SEEK_END)
    SC_STREAM_SEEK = 8,
    //! Tell support (returns current position)
    SC_STREAM_TELL = 16
};

//! Flags for sc-stream
typedef enum _sc_stream_flags sc_stream_flags;

enum _sc_stream_seek
{
    //! Beginning of file
    SC_STREAM_SEEK_SET = 1,
    //! Current position of the file pointer
    SC_STREAM_SEEK_CUR,
    //! End of file
    SC_STREAM_SEEK_END
};

//! Seek origin
typedef enum _sc_stream_seek sc_stream_seek;

// ------------------ Functions ----------------------

/*! Read data from stream.
 * @param stream Stream pointer to read data
 * @param data Data buffer pointer, that will used to store data (fixed size)
 * @param data_len Length of data buffer
 * @param read_bytes Result: number of bytes, that has been read from stream into buffer.
 * @return If data has been read without any errors, then return SC_OK; otherwise return SC_ERROR
 */
sc_result sc_stream_read_data(sc_stream *stream, sc_char *data, sc_uint32 data_len, sc_uint32 *read_bytes);


/*! Returns length of stream in bytes.
 * @param stream Stream pointer to get size
 * @param length Pointer to result container
 * @return If correct length returned, then return SC_OK; otherwise return SC_ERROR
 */
sc_result sc_stream_get_length(sc_stream *stream, sc_uint32 *length);

/*! Returns current position in stream
 * @param stream Stream pointer to get position
 * @param position Pointer to result container
 * @return If correct position returned, thern return SC_OK; otherwise return SC_ERROR
 */
sc_result sc_stream_get_position(sc_stream *stream, sc_uint32 *position);

/*! Check if specified data stream supports specified \p flag
 * @param stream Pointer to data stream for checking
 * @param flag Data stream flag to check
 * @return If specified \p flag is supported by \p stream, then return SC_TRUE; otherwise return SC_FALSE
 */
sc_bool sc_stream_check_flag(sc_stream *stream, sc_uint8 flag);


#endif
