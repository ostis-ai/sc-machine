/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

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

#include "ui.h"
#include "sc_helper.h"

#include "ui_translators.h"
#include "ui_commands.h"

#include <glib.h>

sc_addr *ui_keynodes = 0;

sc_bool initialize_keynodes()
{
    sc_bool result = SC_TRUE;
    ui_keynodes = g_new0(sc_addr, UI_KEYNODE_COUNT);

    if (sc_helper_resolve_system_identifier("ui_command_initiated", &(ui_keynodes[UI_KEYNODE_COMMAND_INITIATED])) == SC_FALSE)
        result = SC_FALSE;


    return SC_TRUE;
}

sc_result initialize()
{

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

    return SC_RESULT_OK;
}

sc_addr ui_get_keynode(ui_keynodes_enum keynode)
{
    g_assert(ui_keynodes != nullptr);
    return ui_keynodes[keynode];
}
