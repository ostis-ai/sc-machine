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
#include "../common/sc_keynodes.h"
#include "utils.h"
#include <glib.h>

const char keynode_nrel_idtf_str[] = "nrel_idtf";
const char keynode_nrel_main_idtf_str[] = "nrel_main_idtf";
const char keynode_nrel_system_identifier_str[] = "nrel_system_identifier";
const char keynode_system_element_str[] = "system_element";

const char keynode_sc_garbage_str[] = "sc_garbage";

sc_addr keynode_nrel_idtf;
sc_addr keynode_nrel_main_idtf;
sc_addr keynode_nrel_system_identifier;
sc_addr keynode_system_element;

sc_addr keynode_sc_garbage;

sc_result utils_collect_keynodes_initialize()
{
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_idtf);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_main_idtf);
    RESOLVE_KEYNODE(s_default_ctx, keynode_nrel_system_identifier);
    RESOLVE_KEYNODE(s_default_ctx, keynode_system_element);

    return SC_RESULT_OK;
}

sc_result utils_keynodes_initialize()
{
    RESOLVE_KEYNODE(s_default_ctx, keynode_sc_garbage);

    return SC_RESULT_OK;
}
