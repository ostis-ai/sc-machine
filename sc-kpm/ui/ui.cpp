/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "uiPrecompiled.h"
#include "ui.h"

#include "uiTranslators.h"
#include "uiCommands.h"
#include "uiKeynodes.h"

extern "C"
{
#include <glib.h>
}

const sc_version required_version = {0, 1, 0, ""};
sc_memory_context * s_default_ctx = 0;

// ------------------- Module ------------------------------
_SC_EXT_EXTERN sc_result initialize()
{
    s_default_ctx = sc_memory_context_new(sc_access_lvl_make_min); 
    
    if (!initialize_keynodes())
    {
        g_warning("Some errors, while initialize ui keynodes");
        return SC_RESULT_ERROR;
    }

    ui_initialize_commands();
    ui_initialize_translators();

    return SC_RESULT_OK;
}

_SC_EXT_EXTERN sc_result shutdown()
{

    ui_shutdown_translators();
    ui_shutdown_commands();

    sc_memory_context_free(s_default_ctx);

    return SC_RESULT_OK;
}
