/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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

#ifndef _sc_fm_redis_config_h_
#define _sc_fm_redis_config_h_

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
