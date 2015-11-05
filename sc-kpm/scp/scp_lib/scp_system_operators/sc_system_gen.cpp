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
extern "C"
{
#include "sc_memory_headers.h"
}
#include "sc_system_gen.h"

#include <stdio.h>
#include <iostream>


sc_addr gen_correspondence_for_var(sc_memory_context *context, sc_addr var_elem, sc_type_result *params, sc_type_result *result)
{
    sc_type_result::iterator pos = params->find(var_elem);
    if (pos != params->end())
    {
        return (*pos).second;
    }
    sc_type var_type;
    sc_memory_get_element_type(context, var_elem, &var_type);
    sc_type const_type = ((~sc_type_var & var_type) | sc_type_const);
    sc_addr const_elem;
    if ((sc_type_node & var_type) == sc_type_node)
    {
        const_elem = sc_memory_node_new(context, const_type);
    }
    else
    {
        sc_addr var_begin, var_end;
        sc_memory_get_arc_begin(context, var_elem, &var_begin);
        sc_memory_get_arc_end(context, var_elem, &var_end);
        sc_addr const_begin = gen_correspondence_for_var(context, var_begin, params, result);
        sc_addr const_end = gen_correspondence_for_var(context, var_end, params, result);
        const_elem = sc_memory_arc_new(context, const_type, const_begin, const_end);
    }
    params->insert(sc_addr_pair(var_elem, const_elem));
    result->insert(sc_addr_pair(var_elem, const_elem));
    return const_elem;
}

sc_result system_sys_gen(sc_memory_context *context, sc_addr pattern, sc_type_result params, sc_type_result *result)
{
    sc_type_result all_pairs;
    all_pairs.insert(params.begin(), params.end());
    sc_iterator3 *it = sc_iterator3_f_a_a_new(context, pattern, 0, sc_type_node | sc_type_const);
    while (SC_TRUE == sc_iterator3_next(it))
    {
        sc_addr node = sc_iterator3_value(it, 2);
        all_pairs.insert(sc_addr_pair(node, node));
    }
    sc_iterator3_free(it);

    it = sc_iterator3_f_a_a_new(context, pattern, 0, sc_type_var);
    while (SC_TRUE == sc_iterator3_next(it))
    {
        sc_addr node = sc_iterator3_value(it, 2);
        gen_correspondence_for_var(context, node, &all_pairs, result);
    }
    sc_iterator3_free(it);

    //print_result(*result);

    return SC_RESULT_OK;
}

sc_result system_sys_gen_for_variables(sc_memory_context *context, sc_addr pattern, sc_type_result params, sc_addr_vector requested_values, sc_type_result *result)
{
    sc_type_result all_pairs;
    all_pairs.insert(params.begin(), params.end());
    sc_iterator3 *it = sc_iterator3_f_a_a_new(context, pattern, 0, sc_type_node | sc_type_const);
    while (SC_TRUE == sc_iterator3_next(it))
    {
        sc_addr node = sc_iterator3_value(it, 2);
        all_pairs.insert(sc_addr_pair(node, node));
    }
    sc_iterator3_free(it);

    it = sc_iterator3_f_a_a_new(context, pattern, 0, sc_type_var);
    while (SC_TRUE == sc_iterator3_next(it))
    {
        sc_addr node = sc_iterator3_value(it, 2);
        gen_correspondence_for_var(context, node, &all_pairs, result);
    }
    sc_iterator3_free(it);

    filter_result_by_variables(result, &requested_values);

    //print_result(*result);

    return SC_RESULT_OK;
}
