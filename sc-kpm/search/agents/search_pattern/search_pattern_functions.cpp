/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

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

extern "C" {
#include <sc_memory_headers.h>
#include <search_defines.h>
#include <search_keynodes.h>
#include <sc_helper.h>
#include <glib.h>
}

#include <agents/search_pattern/search_pattern_functions.h>
#include <agents/search_pattern/sc_system_search.h>

extern "C" sc_result search_full_pattern(sc_addr pattern, sc_addr answer, sc_bool sys_off)
{
    sc_type_result params;
    sc_addr addr2, arc;
    sc_type_result_vector result;
    sc_type_result::iterator it;
    sc_uint i;
    sc_result res = SC_RESULT_OK;
    GHashTable *table;

    if (SC_RESULT_OK != system_sys_search_only_full(pattern, params, &result))
    {
        return SC_RESULT_ERROR;
    }
    if (result.size() > 0)
    {
        res = SC_RESULT_OK;
    }
    else
    {
        free_result_vector(&result);
        return SC_RESULT_ERROR;
    }

    table = g_hash_table_new(NULL, NULL);

    for (i = 0; i < result.size(); i++)
    {
        for (it = result[i]->begin() ; it != result[i]->end(); it++)
        {
            addr2 = (*it).second;
            if (FALSE == g_hash_table_contains(table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr2))))
            {
                g_hash_table_add(table, GINT_TO_POINTER(SC_ADDR_LOCAL_TO_INT(addr2)));
                if (sys_off == SC_TRUE && IS_SYSTEM_ELEMENT(addr2))
                    continue;

                arc = sc_memory_arc_new(sc_type_arc_pos_const_perm, answer, addr2);
                SYSTEM_ELEMENT(arc);
            }
        }
    }

    free_result_vector(&result);
    g_hash_table_destroy(table);
    return res;
}
