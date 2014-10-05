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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
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
sc_result initialize()
{
    s_default_ctx = sc_memory_context_new(sc_access_lvl_make_min);

    if (sc_version_compare(&SC_VERSION, &required_version) < 0)
    {
        char *req_str = sc_version_string_new(&required_version);
        char *v_str = sc_version_string_new(&SC_VERSION);

        g_error("Required version %s, but you use %s", req_str, v_str);

        sc_version_string_free(req_str);
        sc_version_string_free(v_str);

        return SC_RESULT_ERROR;
    }

    if (!initialize_keynodes())
    {
        g_warning("Some errors, while initialize ui keynodes");
        return SC_RESULT_ERROR;
    }

    ui_initialize_commands();
    ui_initialize_translators();

    return SC_RESULT_OK;
}

sc_result shutdown()
{

    ui_shutdown_translators();
    ui_shutdown_commands();

    sc_memory_context_free(s_default_ctx);

    return SC_RESULT_OK;
}
