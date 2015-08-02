/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_stream_redis_h_
#define _sc_stream_redis_h_

#include "sc_fm_redis_types.h"

#include "sc_stream.h"
#include <hiredis/hiredis.h>
#include <glib.h>

/*! Create redis value data stream
 * @param context Pointer to used redis context
 * @param key Redis key for streaming
 * @param flags Data stream flags
 * @param mutex Pointer to synchronization mutex
 * @remarks Allocate and create redis value data stream. The returned stream pointer should be freed
 * with sc_stream_free function, when done using it.
 * @return Returns stream pointer if the stream was successfully created, or NULL if an error occurred
 */
sc_stream* sc_stream_redis_new(redisContext *context, const sc_char *key, sc_uint8 flags, GMutex *mutex);


#endif // _sc_stream_redis_h_
