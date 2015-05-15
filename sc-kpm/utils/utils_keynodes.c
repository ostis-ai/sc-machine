/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
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
