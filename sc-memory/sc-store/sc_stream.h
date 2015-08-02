/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_stream_h_
#define _sc_stream_h_

#include "sc_types.h"

//! Type of data stream
typedef struct _sc_stream sc_stream;

enum _sc_stream_flags
{
    //! Data can be read from stream
    SC_STREAM_FLAG_READ = 1,
    //! Data can be written into stream
    SC_STREAM_FLAG_WRITE = 2,
    //! Stream opened for appending (compatible just with SC_STREAM_WRITE flag)
    SC_STREAM_FLAG_APPEND = 4,
    //! Seek support (SEEK_SET, SEEK_CUR, SEEK_END)
    SC_STREAM_FLAG_SEEK = 8,
    //! Tell support (returns current position)
    SC_STREAM_FLAG_TELL = 16
};

//! Flags for sc-stream
typedef enum _sc_stream_flags sc_stream_flags;

enum _sc_stream_seek_origin
{
    //! Beginning of file
    SC_STREAM_SEEK_SET = 1,
    //! Current position of the file pointer
    SC_STREAM_SEEK_CUR,
    //! End of file
    SC_STREAM_SEEK_END
};

//! Seek origin
typedef enum _sc_stream_seek_origin sc_stream_seek_origin;

// ------------------ Functions ----------------------
/*! Free specified stream
 * @param stream Pointer to data stream for freeing
 * @return If stream has been freed without any errors, then return SC_OK; otherwise return SC_ERROR
 * @remarks After calling this function \p stream pointer woldn't be a valid.
 */
_SC_EXTERN sc_result sc_stream_free(sc_stream *stream);

/*! Read data from stream.
 * @param stream Stream pointer to read data
 * @param data Data buffer pointer, that will used to store data (fixed size)
 * @param data_len Length of data buffer
 * @param read_bytes Result: number of bytes, that has been read from stream into buffer.
 * @return If data has been read without any errors, then return SC_OK; otherwise return SC_ERROR
 */
_SC_EXTERN sc_result sc_stream_read_data(const sc_stream *stream, sc_char *data, sc_uint32 data_len, sc_uint32 *read_bytes);

/*! Write data into stream
 * @param stream Stream pointer to write data
 * @param data Pointer to buffer, that contains data for writing
 * @param data_len Length of data in buffer
 * @param written_bytes Result: Number of bytes that has been written into stream
 * @return If data has been written without any errors, then return SC_OK; otherwise return SC_ERROR
 */
_SC_EXTERN sc_result sc_stream_write_data(const sc_stream *stream, sc_char *data, sc_uint32 data_len, sc_uint32 *written_bytes);

/*! Sets the position indicator associated with the stream to a new position
 * @param stream Stream pointer to change position
 * @param seek_origin Initial position for seeking (@see sc_stream_seek_origin)
 * @param offset Offset in bytes
 * @return If position changed without any errors, then return SC_OK; otherwise return SC_ERROR
 */
_SC_EXTERN sc_result sc_stream_seek(const sc_stream *stream, sc_stream_seek_origin seek_origin, sc_uint32 offset);

/*! Check if stream points into the end position
 * @param stream Stream pointer to check
 * @return If stream points into the end position, then return SC_TRUE; otherwise return SC_FALSE
 */
_SC_EXTERN sc_bool sc_stream_eof(const sc_stream *stream);


/*! Returns length of stream in bytes.
 * @param stream Stream pointer to get size
 * @param length Pointer to result container
 * @return If correct length returned, then return SC_OK; otherwise return SC_ERROR
 */
_SC_EXTERN sc_result sc_stream_get_length(const sc_stream *stream, sc_uint32 *length);

/*! Returns current position in stream
 * @param stream Stream pointer to get position
 * @param position Pointer to result container
 * @return If correct position returned, thern return SC_OK; otherwise return SC_ERROR
 */
_SC_EXTERN sc_result sc_stream_get_position(const sc_stream *stream, sc_uint32 *position);

/*! Check if specified data stream supports specified \p flag
 * @param stream Pointer to data stream for checking
 * @param flag Data stream flag to check
 * @return If specified \p flag is supported by \p stream, then return SC_TRUE; otherwise return SC_FALSE
 */
_SC_EXTERN sc_bool sc_stream_check_flag(const sc_stream *stream, sc_uint8 flag);


#endif
