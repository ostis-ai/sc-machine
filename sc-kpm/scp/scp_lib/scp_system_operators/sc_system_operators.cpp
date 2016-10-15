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
#include "sc_helper.h"
}
#include "sc_system_operators.h"

#include <stdio.h>
#include <iostream>
#include <set>
#include <algorithm>

void printIdtf(sc_memory_context *context, sc_addr element)
{
    sc_addr idtf;
    sc_type type;
    sc_memory_get_element_type(context, element, &type);

    if ((sc_type_node & type) == sc_type_node || (sc_type_link & type) == sc_type_link )
    {

		if (SC_RESULT_OK == sc_helper_get_system_identifier_link(context, element, &idtf))
        {
            sc_stream *stream;
            sc_uint32 length = 0, read_length = 0;
            sc_char *data;
            sc_memory_get_link_content(context, idtf, &stream);
            sc_stream_get_length(stream, &length);
            data = (sc_char *)calloc(length + 1, sizeof(sc_char));
            sc_stream_read_data(stream, data, length, &read_length);
            data[length] = '\0';
            printf("%s", data);
            sc_stream_free(stream);
            free(data);
        }
        else
        {
            printf("%u|%u", element.seg, element.offset);
        }
    }
    else
    {
        sc_addr elem1, elem2;
        sc_memory_get_arc_begin(context, element, &elem1);
        sc_memory_get_arc_end(context, element, &elem2);
        printf("(");
        printIdtf(context, elem1);
        printf(" -> ");
        printIdtf(context, elem2);
        printf(")");
    }
}

bool sc_result_comparator(sc_type_result *s1, sc_type_result *s2)
{
    return (s1->size() > s2->size());
}

void print_hash(sc_type_hash table)
{
    sc_type_hash::iterator it;
    printf("HASHMAP:\n");
    for (it = table.begin() ; it != table.end(); it++)
    {
        sc_addr addr = (*it).second;
        std::cout << addr.seg << "|" << addr.offset << std::endl;
    }
}

void print_result(sc_memory_context *context, sc_type_result table)
{
    sc_type_result::iterator it;
    printf("RESULT (%d):\n", (int)table.size());
    for (it = table.begin() ; it != table.end(); it++)
    {
        sc_addr addr1 = (*it).first;
        sc_addr addr2 = (*it).second;
        printIdtf(context, addr1);
        std::cout << " => ";
        printIdtf(context, addr2);
        std::cout << std::endl;
        //std::cout << addr1.seg << "|" << addr1.offset << "=>" << addr2.seg << "|" << addr2.offset << std::endl;
    }
}

void print_result_set(sc_memory_context *context, sc_type_result_vector *table)
{
    printf("RESULT COUNT:%d\n", (int)table->size());
    for (sc_uint i = 0; i < table->size(); i++)
    {
        print_result(context, *((*table)[i]));
    }
}

sc_bool copy_set_into_hash(sc_memory_context *context, sc_addr set, sc_type arc_type, sc_type end_type, sc_type_hash *table, sc_uint *var_count)
{
    sc_addr addr3;
    sc_iterator3 *it = sc_iterator3_f_a_a_new(context, set, arc_type, end_type);
    if (it == 0)
    {
        return SC_FALSE;
    }
    while (SC_TRUE == sc_iterator3_next(it))
    {
        addr3 = sc_iterator3_value(it, 2);
        table->insert(sc_hash_pair(SC_ADDR_LOCAL_TO_INT(addr3), addr3));

        sc_type element_type;
        if (sc_memory_get_element_type(context, addr3, &element_type) != SC_RESULT_OK) {continue;}
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

sc_bool check_coherence(sc_memory_context *context, sc_addr element, sc_addr sc_pattern, sc_addr checked_arc, sc_bool output, sc_type_result *result, sc_type_result *inp_result)
{
    sc_addr addr1, addr2, temp;

    sc_iterator3 *it_pattern_arc = sc_iterator3_f_a_a_new(context, element, 0, 0);
    if (it_pattern_arc == null_ptr) {return SC_FALSE;}
    while (SC_TRUE == sc_iterator3_next(it_pattern_arc))
    {
        addr2 = sc_iterator3_value(it_pattern_arc, 1);
        if (SC_TRUE == sc_helper_check_arc(context, sc_pattern, addr2, sc_type_arc_pos_const_perm))
        {
            if (SC_TRUE == output && SC_ADDR_IS_EQUAL(checked_arc, addr2))
            {
                continue;
            }

            if (SC_TRUE == find_result_pair_for_var(result, addr2, &temp) &&
                SC_FALSE == find_result_pair_for_var(inp_result, addr2, &temp))
            {
                sc_iterator3_free(it_pattern_arc);
                return SC_FALSE;
            }
        }
    }
    sc_iterator3_free(it_pattern_arc);

    it_pattern_arc = sc_iterator3_a_a_f_new(context, 0, 0, element);
    if (it_pattern_arc == null_ptr) {return SC_FALSE;}
    while (SC_TRUE == sc_iterator3_next(it_pattern_arc))
    {
        addr1 = sc_iterator3_value(it_pattern_arc, 0);
        if (SC_ADDR_IS_EQUAL(addr1, sc_pattern)) continue;
        addr2 = sc_iterator3_value(it_pattern_arc, 1);
        if (SC_TRUE == sc_helper_check_arc(context, sc_pattern, addr2, sc_type_arc_pos_const_perm))
        {
            if (SC_FALSE == output && SC_ADDR_IS_EQUAL(checked_arc, addr2))
            {
                continue;
            }

            if (SC_TRUE == find_result_pair_for_var(result, addr2, &temp) &&
                SC_FALSE == find_result_pair_for_var(inp_result, addr2, &temp))
            {
                sc_iterator3_free(it_pattern_arc);
                return SC_FALSE;
            }
        }
    }
    sc_iterator3_free(it_pattern_arc);

    sc_type element_type;

    if (SC_RESULT_OK != sc_memory_get_element_type(context, element, &element_type)) {return SC_FALSE;}
    if ((sc_type_node & element_type) != sc_type_node)
    {
        sc_addr end, begin;
        if (SC_RESULT_OK != sc_memory_get_arc_begin(context, element, &begin)) {return SC_FALSE;}
        if (SC_RESULT_OK != sc_memory_get_arc_end(context, element, &end)) {return SC_FALSE;}

        if (SC_TRUE == check_coherence(context, begin, sc_pattern, element, SC_TRUE, result, inp_result) &&
            SC_TRUE == check_coherence(context, end, sc_pattern, element, SC_FALSE, result, inp_result))
            return SC_TRUE;
        else
            return SC_FALSE;
    }
    return SC_TRUE;
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
