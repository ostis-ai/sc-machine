/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_stream_file_h_
#define _sc_stream_file_h_

#include "sc_stream.h"

/*! Create file data stream
 * @param file_name Path to file for streaming
 * @param flags Data stream flags
 * @remarks Allocate and create file data stream. The returned stream pointer should be freed
 * with sc_stream_free function, when done using it.
 * @return Returns stream pointer if the stream was successfully created, or NULL if an error occurred
 */
_SC_EXTERN sc_stream* sc_stream_file_new(const sc_char *file_name, sc_uint8 flags);


#endif
