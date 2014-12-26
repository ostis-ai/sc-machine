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

#include "sc_keynodes.h"

#include <glib.h>

sc_result sc_common_resolve_keynode(sc_memory_context const * ctx, char const * sys_idtf, sc_addr * keynode)
{
    if (sc_helper_resolve_system_identifier(ctx, sys_idtf, keynode) == SC_FALSE)
    {
        *keynode = sc_memory_node_new(ctx, 0);
        if (sc_helper_set_system_identifier(ctx, *keynode, sys_idtf, strlen(sys_idtf)) != SC_RESULT_OK)
            return SC_RESULT_ERROR;
        g_message("Created element with system identifier: %s", sys_idtf);
    }
    return SC_RESULT_OK;
}
