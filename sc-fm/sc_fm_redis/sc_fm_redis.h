/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_fm_redis_h_
#define _sc_fm_redis_h_

#include "sc_fm_redis_types.h"
#include <hiredis/hiredis.h>

redisReply* do_sync_redis_command(redisContext **context, const char *format, ...);

#endif
