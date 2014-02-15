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
#include "utils_keynodes.h"
#include <glib.h>

const char keynode_nrel_idtf_str[] = "nrel_idtf";
const char keynode_nrel_main_idtf_str[] = "nrel_main_idtf";
const char keynode_nrel_system_identifier_str[] = "nrel_system_identifier";

sc_addr keynode_nrel_idtf;
sc_addr keynode_nrel_main_idtf;
sc_addr keynode_nrel_system_identifier;

#define resolve_keynode(keynode) \
    if (sc_helper_resolve_system_identifier(keynode##_str, &keynode) == SC_FALSE) \
    {\
        g_warning("Can't find element with system identifier: %s", keynode##_str); \
        keynode = sc_memory_node_new(0); \
        if (sc_helper_set_system_identifier(keynode, keynode##_str, strlen(keynode##_str)) != SC_RESULT_OK) \
            return SC_RESULT_ERROR; \
        g_message("Created element with system identifier: %s", keynode##_str); \
    }

sc_result utils_keynodes_initialize()
{
    resolve_keynode(keynode_nrel_idtf);
    resolve_keynode(keynode_nrel_main_idtf);
    resolve_keynode(keynode_nrel_system_identifier);

    return SC_RESULT_OK;
}
