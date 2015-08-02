/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.h"

#include <glib.h>

sc_result sc_common_resolve_keynode(sc_memory_context const * ctx, char const * sys_idtf, sc_addr * keynode)
{
    if (sc_helper_resolve_system_identifier(ctx, sys_idtf, keynode) == SC_FALSE)
    {
        *keynode = sc_memory_node_new(ctx, 0);
        if (sc_helper_set_system_identifier(ctx, *keynode, sys_idtf, (sc_uint32)strlen(sys_idtf)) != SC_RESULT_OK)
            return SC_RESULT_ERROR;
        g_message("Created element with system identifier: %s", sys_idtf);
    }
    return SC_RESULT_OK;
}
