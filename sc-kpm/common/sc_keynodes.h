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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS. If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
 */

#ifndef _sc_keynodes_h_
#define _sc_keynodes_h_

#include "sc_helper.h"
#include "sc_memory.h"

sc_result sc_common_resolve_keynode(sc_memory_context const * ctx, char const * sys_idtf, sc_addr * keynode);

#define RESOLVE_KEYNODE(ctx, keynode) if (sc_common_resolve_keynode(ctx, keynode##_str, &keynode) != SC_RESULT_OK) return SC_RESULT_ERROR;

#endif
