#ifndef _sc_stream_file_h_
#define _sc_stream_file_h_

#include "sc_stream.h"

/*! Create file data stream
 * @param file_name Path to file for streaming
 * @param flags Data stream flags
 * @remarks Allocate and create file data stream. The returned stream pointer should be freed
 * with sc_stream_free function, when done using it.
 */
sc_stream* sc_stream_file_new(sc_char *file_name, sc_uint8 flags);

#endif
