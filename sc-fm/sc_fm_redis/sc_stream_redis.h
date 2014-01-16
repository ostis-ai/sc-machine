/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2014 OSTIS

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

#ifndef _sc_stream_redis_h_
#define _sc_stream_redis_h_


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
