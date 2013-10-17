#include "uiPrecompiled.h"
#include "uiUtils.h"

sc_result ui_check_cmd_type(sc_addr cmd_addr, sc_addr cmd_class)
{
    if (sc_helper_check_arc(cmd_class, cmd_addr, sc_type_arc_pos_const_perm) == SC_TRUE)
        return SC_RESULT_OK;

    return SC_RESULT_ERROR;
}
