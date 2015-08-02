/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_fm_redis_config_h_
#define _sc_fm_redis_config_h_

#include "sc_fm_redis_types.h"
#include "sc_config.h"

//! Initialize redis configuration
void sc_redis_config_initialize();

//! Shutting down redis configuration
void sc_redis_config_shutdown();

//! Returns redis server host address
const sc_uint8* sc_redis_config_host();
//! Returns redis server port
sc_uint32 sc_redis_config_port();
//! Returns milliseconds for commands timeout
sc_uint32 sc_redis_config_timeout();


#endif
