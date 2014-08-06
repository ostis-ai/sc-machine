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
#include "sc_system_operators.h"
#include "search.h"

#include <stdio.h>
#include <iostream>
#include <set>
#include <algorithm>

bool sc_result_comparator(sc_type_result *s1, sc_type_result *s2)
{
    return (s1->size() > s2->size());
}

sc_bool copy_set_into_hash(sc_addr set, sc_type arc_type, sc_type end_type, sc_type_hash *table, sc_uint *var_count)
{
    sc_addr addr3;
    sc_iterator3 *it = sc_iterator3_f_a_a_new(s_default_ctx,
                                              set,
                                              arc_type,
                                              end_type);
    if (it == 0)
    {
        return SC_FALSE;
    }
    while (SC_TRUE == sc_iterator3_next(it))
    {
        addr3 = sc_iterator3_value(it, 2);
        table->insert(sc_hash_pair(SC_ADDR_LOCAL_TO_INT(addr3), addr3));

        sc_type element_type;
        if (sc_memory_get_element_type(s_default_ctx, addr3, &element_type) != SC_RESULT_OK) {continue;}
        if ((sc_type_var & element_type) == sc_type_var)
        {
            (*var_count)++;
        }
    }
    sc_iterator3_free(it);
    return SC_TRUE;
}

sc_bool find_result_pair_for_var(sc_type_result *set, sc_addr var_element, sc_addr *result)
{
    sc_type_result::iterator it = set->find(var_element);
    if (it == set->end())
    {
        return SC_FALSE;
    }
    else
    {
        *result = (*it).second;
        return SC_TRUE;
    }
}

sc_bool find_result_pair_for_const(sc_type_result *set, sc_addr const_element, sc_addr *result)
{
    sc_type_result::iterator it;
    for (it = set->begin() ; it != set->end(); it++)
    {
        sc_addr var = (*it).first;
        sc_addr cnst = (*it).second;
        if (SC_ADDR_IS_EQUAL(cnst, const_element))
        {
            *result = var;
            return SC_TRUE;
        }
    }
    return SC_FALSE;
}

void cantorize_result_vector(sc_type_result_vector *result)
{
    std::set<sc_type_result *> myset(result->begin(), result->end());
    result->clear();
    result->insert(result->begin(), myset.begin(), myset.end());
}

void remove_all_elements(sc_type_result_vector *pattern, sc_type_result_vector *source)
{
    for (sc_uint i = 0; i < pattern->size(); i++)
    {
        source->erase(std::remove(source->begin(), source->end(), (*pattern)[i]), source->end());
    }
}

void free_single_result(sc_type_result *result)
{
    delete result;
    //class_count--;
}

void free_result_vector(sc_type_result_vector *result)
{
    for (sc_uint i = 0; i < result->size(); i++)
    {
        free_single_result((*result)[i]);
    }
    result->clear();
}

void sort_result_vector(sc_type_result_vector *data)
{
    std::sort(data->begin(), data->end(), sc_result_comparator);
}

sc_bool remove_result_vector_short_results(sc_type_result_vector *data)
{
    sc_type_result_vector::iterator it;
    for (it = data->begin() + 1 ; it != data->end(); it++)
    {
        if (sc_result_comparator(*(it - 1), *it))
        {
            sc_type_result_vector clear_data(it, data->end());
            data->erase(it, data->end());
            free_result_vector(&clear_data);
            return SC_TRUE;
        }
    }
    return SC_FALSE;
}

sc_bool remove_result_vector_short_results(sc_type_result_vector *data, sc_uint size)
{
    sc_type_result_vector::iterator it;
    for (it = data->begin() ; it != data->end(); it++)
    {
        if ((*it)->size() < size)
        {
            sc_type_result_vector clear_data(it, data->end());
            data->erase(it, data->end());
            free_result_vector(&clear_data);
            return SC_TRUE;
        }
    }
    return SC_FALSE;
}

sc_bool sc_addr_vector_contains(sc_addr addr, sc_addr_vector *requested_values)
{
    for (sc_uint i = 0; i < requested_values->size(); i++)
    {
        sc_addr addr1 = (*requested_values)[i];
        if (SC_ADDR_IS_EQUAL(addr, addr1))
            return SC_TRUE;
    }
    return SC_FALSE;
}

void filter_result_vector_by_variables(sc_type_result_vector *data, sc_addr_vector *requested_values)
{
    sc_type_result_vector::iterator it;
    sc_type_result::iterator it1;

    for (it = data->begin() ; it != data->end(); it++)
    {
        for (it1 = (*it)->begin() ; it1 != (*it)->end(); it1++)
        {
            sc_addr key = (*it1).first;
            if (SC_FALSE == sc_addr_vector_contains(key, requested_values))
            {
                (*it)->erase(key);
            }
        }
    }
}

void filter_result_by_variables(sc_type_result *data, sc_addr_vector *requested_values)
{
    sc_type_result::iterator it;
    for (it = data->begin() ; it != data->end(); it++)
    {
        sc_addr key = (*it).first;
        if (SC_FALSE == sc_addr_vector_contains(key, requested_values))
        {
            data->erase(key);
        }
    }
}
