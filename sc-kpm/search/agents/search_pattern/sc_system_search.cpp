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
#include "sc_system_search.h"
#include "search.h"

#include <stdio.h>
#include <iostream>


sc_bool system_sys_search_recurse(sc_addr sc_pattern, sc_type_hash pattern, sc_addr curr_const_element, sc_addr curr_pattern_element, sc_type_result *inp_result, sc_type_result_vector *out_common_result, int element_number)
{
    sc_type input_element_type;
    if (sc_memory_get_element_type(s_default_ctx, curr_pattern_element, &input_element_type) != SC_RESULT_OK) {return SC_FALSE;}
    if (element_number == 2 && (sc_type_node & input_element_type) != sc_type_node)
    {
        //!Input element is arc
        sc_addr const_element, pattern_element, temp, end_pattern_element, end_const_element;
        sc_type pattern_element_type, end_pattern_element_type;
        sc_memory_get_arc_begin(s_default_ctx, curr_const_element, &const_element);
        sc_memory_get_arc_begin(s_default_ctx, curr_pattern_element, &pattern_element);
        sc_memory_get_arc_end(s_default_ctx, curr_const_element, &end_const_element);
        sc_memory_get_arc_end(s_default_ctx, curr_pattern_element, &end_pattern_element);

        if (sc_memory_get_element_type(s_default_ctx, pattern_element, &pattern_element_type) != SC_RESULT_OK) {return SC_TRUE;}
        if ((sc_type_const & pattern_element_type) == sc_type_const)
        {
            if (SC_ADDR_IS_NOT_EQUAL(const_element, pattern_element))
            {
                inp_result->erase(curr_pattern_element);
                return SC_TRUE;
            }
        }
        else if (SC_TRUE == find_result_pair_for_var(inp_result, pattern_element, &temp))
        {
            if (SC_ADDR_IS_NOT_EQUAL(const_element, temp))
            {
                inp_result->erase(curr_pattern_element);
                return SC_TRUE;
            }
        }

        if (sc_memory_get_element_type(s_default_ctx, end_pattern_element, &end_pattern_element_type) != SC_RESULT_OK) {return SC_TRUE;}
        if ((sc_type_const & end_pattern_element_type) == sc_type_const)
        {
            if (SC_ADDR_IS_NOT_EQUAL(end_const_element, end_pattern_element))
            {
                inp_result->erase(curr_pattern_element);
                return SC_TRUE;
            }
        }
        else if (SC_TRUE == find_result_pair_for_var(inp_result, end_pattern_element, &temp))
        {
            if (SC_ADDR_IS_NOT_EQUAL(end_const_element, temp))
            {
                inp_result->erase(curr_pattern_element);
                return SC_TRUE;
            }
        }
        if (SC_ADDR_IS_NOT_EQUAL(const_element, pattern_element))
        {
            inp_result->insert(sc_addr_pair(pattern_element, const_element));
        }
        pattern.insert(sc_hash_pair(SC_ADDR_LOCAL_TO_INT(curr_pattern_element), curr_pattern_element));
        system_sys_search_recurse(sc_pattern, pattern, const_element, pattern_element, inp_result, out_common_result, 2);
        return SC_TRUE;
    }

    sc_addr addr1, addr2, temp, temp1;
    int out_arc_count = 0;

    sc_type_result_vector common_result;
    sc_type_result_vector del_result;
    sc_type_result inp_result_copy = *inp_result;
    common_result.push_back(inp_result);

    //Pattern arcs list

    sc_addr_vector pattern_arc_set;

    sc_iterator3 *it_pattern_arc = sc_iterator3_f_a_a_new(s_default_ctx,
                                                          curr_pattern_element,
                                                          0,
                                                          0);
    if (it_pattern_arc == nullptr) {return SC_FALSE;}
    while (SC_TRUE == sc_iterator3_next(it_pattern_arc))
    {
        addr2 = sc_iterator3_value(it_pattern_arc, 1);
        if (pattern.find(SC_ADDR_LOCAL_TO_INT(addr2)) != pattern.end())
        {
            out_arc_count++;
            pattern_arc_set.push_back(addr2);
        }
    }
    sc_iterator3_free(it_pattern_arc);

    it_pattern_arc = sc_iterator3_a_a_f_new(s_default_ctx,
                                            0,
                                            0,
                                            curr_pattern_element);
    if (it_pattern_arc == nullptr) {return SC_FALSE;}
    while (SC_TRUE == sc_iterator3_next(it_pattern_arc))
    {
        addr1 = sc_iterator3_value(it_pattern_arc, 0);
        if (SC_ADDR_IS_EQUAL(addr1, sc_pattern)) continue;
        addr2 = sc_iterator3_value(it_pattern_arc, 1);
        if (pattern.find(SC_ADDR_LOCAL_TO_INT(addr2)) != pattern.end())
        {
            pattern_arc_set.push_back(addr2);
        }
    }
    sc_iterator3_free(it_pattern_arc);

    sc_addr pattern_arc;
    sc_addr const_arc;
    sc_addr next_pattern_element;
    sc_addr next_const_element;
    sc_bool out_arc_flag = SC_TRUE;
    sc_type_result_vector new_common_result;
    sc_type_result_vector next_common_result;
    sc_type_result_vector next_common_result_arc;
    sc_type_result_vector next_common_result1;

    sc_bool pattern_arc_is_const_or_has_value = SC_FALSE;
    sc_bool pattern_is_const_or_has_value = SC_FALSE;

    //Pattern arcs loop
    for (sc_uint i = 0; i < pattern_arc_set.size(); i++)
    {
        pattern_arc = pattern_arc_set[i];
        out_arc_flag = SC_TRUE;

        //!check pattern_arc type
        sc_type pattern_arc_type;
        if (sc_memory_get_element_type(s_default_ctx, pattern_arc, &pattern_arc_type) != SC_RESULT_OK)
            continue;
        if ((sc_type_const & pattern_arc_type) == sc_type_const)
            continue;

        pattern_arc_is_const_or_has_value = SC_FALSE;
        pattern_is_const_or_has_value = SC_FALSE;

        if (out_arc_count > 0)
        {
            if (SC_RESULT_OK != sc_memory_get_arc_end(s_default_ctx, pattern_arc, &next_pattern_element))
                continue;
            out_arc_count--;
        }
        else
        {
            if (SC_RESULT_OK != sc_memory_get_arc_begin(s_default_ctx, pattern_arc, &next_pattern_element))
                continue;
            out_arc_flag = SC_FALSE;
        }

        if (pattern.find(SC_ADDR_LOCAL_TO_INT(next_pattern_element)) == pattern.end())
        {
            continue;
        }

        sc_type_result::iterator arc_it = inp_result_copy.find(pattern_arc);
        if (arc_it != inp_result_copy.end())
        {
            const_arc = (*arc_it).second;
            pattern_arc_is_const_or_has_value = SC_TRUE;
            if (out_arc_flag == SC_TRUE)
            {
                if (SC_RESULT_OK != sc_memory_get_arc_end(s_default_ctx, const_arc, &next_const_element))
                    continue;
            }
            else
            {
                if (SC_RESULT_OK != sc_memory_get_arc_begin(s_default_ctx, const_arc, &next_const_element))
                    continue;
            }
        }

        //!check next_pattern_element type
        sc_type next_pattern_element_type;
        if (sc_memory_get_element_type(s_default_ctx, next_pattern_element, &next_pattern_element_type) != SC_RESULT_OK) {continue;}
        if ((sc_type_const & next_pattern_element_type) == sc_type_const)
        {
            if (pattern_arc_is_const_or_has_value == SC_TRUE)
            {
                if (!SC_ADDR_IS_EQUAL(next_const_element, next_pattern_element))
                {
                    continue;
                }
            }
            else
            {
                next_const_element = next_pattern_element;
            }
            pattern_is_const_or_has_value = SC_TRUE;
        }
        else
        {
            sc_type_result::iterator it = inp_result_copy.find(next_pattern_element);
            if (it != inp_result_copy.end())
            {
                if (pattern_arc_is_const_or_has_value == SC_TRUE)
                {
                    if (!SC_ADDR_IS_EQUAL(next_const_element, (*it).second))
                    {
                        continue;
                    }
                }
                else
                {
                    next_const_element = (*it).second;
                }
                pattern_is_const_or_has_value = SC_TRUE;
            }
        }

        pattern.erase(SC_ADDR_LOCAL_TO_INT(next_pattern_element));

        //!const arc loop
        sc_addr_vector const_arc_set;

        if (pattern_arc_is_const_or_has_value == SC_FALSE)
        {
            sc_iterator3 *it_const_arc;
            sc_type const_arc_type = ((~sc_type_var & pattern_arc_type) | sc_type_const);
            if (out_arc_flag == SC_TRUE)
            {
                if (pattern_is_const_or_has_value == SC_TRUE)
                {
                    it_const_arc = sc_iterator3_f_a_f_new(s_default_ctx,
                                                          curr_const_element,
                                                          const_arc_type,
                                                          next_const_element);
                }
                else
                {
                    sc_type next_const_element_type = ((~sc_type_var & next_pattern_element_type) | sc_type_const);
                    it_const_arc = sc_iterator3_f_a_a_new(s_default_ctx,
                                                          curr_const_element,
                                                          const_arc_type,
                                                          next_const_element_type);
                }
            }
            else
            {
                if (pattern_is_const_or_has_value == SC_TRUE)
                {
                    it_const_arc = sc_iterator3_f_a_f_new(s_default_ctx,
                                                          next_const_element,
                                                          const_arc_type,
                                                          curr_const_element);
                }
                else
                {
                    sc_type next_const_element_type = ((~sc_type_var & next_pattern_element_type) | sc_type_const);
                    it_const_arc = sc_iterator3_a_a_f_new(s_default_ctx,
                                                          next_const_element_type,
                                                          const_arc_type,
                                                          curr_const_element);
                }
            }
            while (SC_TRUE == sc_iterator3_next(it_const_arc))
            {
                if (out_arc_flag == SC_FALSE)
                {
                    addr1 = sc_iterator3_value(it_const_arc, 0);
                    if (SC_ADDR_IS_EQUAL(addr1, sc_pattern))
                        continue;
                }
                addr2 = sc_iterator3_value(it_const_arc, 1);
                const_arc_set.push_back(addr2);
            }
            sc_iterator3_free(it_const_arc);
        }
        else
        {
            const_arc_set.push_back(const_arc);
        }

        //printf("ELEMENT %u|%u CONST ARCS COUNT:%d\n", curr_const_element.seg, curr_const_element.offset, const_arc_set.size());

        for (sc_uint j = 0; j < const_arc_set.size(); j++)
        {
            const_arc = const_arc_set[j];

            if (pattern_arc_is_const_or_has_value == SC_FALSE)
            {
                if (out_arc_flag == SC_TRUE)
                {
                    if (SC_RESULT_OK != sc_memory_get_arc_end(s_default_ctx, const_arc, &next_const_element))
                        continue;
                }
                else
                {
                    if (SC_RESULT_OK != sc_memory_get_arc_begin(s_default_ctx, const_arc, &next_const_element))
                        continue;
                }
            }

            if (pattern_is_const_or_has_value == SC_FALSE && pattern.find(SC_ADDR_LOCAL_TO_INT(next_const_element)) != pattern.end())
            {
                continue;
            }

            //!Results loop
            for (sc_uint k = 0; k < common_result.size(); k++)
            {
                sc_type_result *result = common_result[k];
                sc_bool gen_arc = SC_FALSE;

                if (pattern_arc_is_const_or_has_value == SC_FALSE)
                {
                    if (SC_TRUE == find_result_pair_for_const(result, const_arc, &temp))
                        continue;


                    if (SC_TRUE == find_result_pair_for_var(result, pattern_arc, &temp))
                    {
                        //!Gen new result
                        if (SC_FALSE == pattern_is_const_or_has_value
                            && SC_TRUE == find_result_pair_for_const(result, next_const_element, &temp1)
                            && SC_ADDR_IS_NOT_EQUAL(temp1, next_pattern_element))
                        {
                            continue;
                        }
                        sc_type_result *new_result = new sc_type_result();
                        //class_count++;
                        (*new_result) = (*result);
                        new_common_result.push_back(new_result);
                        result = new_result;
                        result->erase(pattern_arc);
                        if (pattern_is_const_or_has_value == SC_FALSE)
                        {
                            result->erase(next_pattern_element);
                        }
                    }

                    gen_arc = SC_TRUE;
                }

                //!Genering pair for 3rd element
                if (pattern_is_const_or_has_value == SC_FALSE
                    && SC_FALSE == find_result_pair_for_var(result, next_pattern_element, &temp))
                {
                    //!Genering pair for 2nd element
                    if (gen_arc == SC_TRUE)
                    {
                        result->insert(sc_addr_pair(pattern_arc, const_arc));
                        pattern.erase(SC_ADDR_LOCAL_TO_INT(pattern_arc));
                    }
                    result->insert(sc_addr_pair(next_pattern_element, next_const_element));
                    pattern.erase(SC_ADDR_LOCAL_TO_INT(next_pattern_element));
                }
                else
                {
                    //!Genering pair for 2nd element
                    if (gen_arc == SC_TRUE)
                    {
                        result->insert(sc_addr_pair(pattern_arc, const_arc));
                        pattern.erase(SC_ADDR_LOCAL_TO_INT(pattern_arc));
                    }
                }

                sc_type_result *recurse_result = new sc_type_result();
                (*recurse_result) = (*result);
                del_result.push_back(result);

                system_sys_search_recurse(sc_pattern, pattern, const_arc, pattern_arc,
                                          recurse_result, &next_common_result_arc, 1);

                for (sc_uint kk = 0; kk < next_common_result_arc.size(); kk++)
                {
                    sc_type_result *element_result = next_common_result_arc[kk];
                    system_sys_search_recurse(sc_pattern, pattern, next_const_element, next_pattern_element,
                                              element_result, &next_common_result1, 2);
                    if (!next_common_result1.empty())
                    {
                        next_common_result.insert(next_common_result.end(),
                                                  next_common_result1.begin(), next_common_result1.end());
                        next_common_result1.clear();
                    }
                    else
                    {
                        next_common_result.push_back(element_result);
                    }
                }

                pattern.insert(sc_hash_pair(SC_ADDR_LOCAL_TO_INT(pattern_arc), pattern_arc));
                pattern.insert(sc_hash_pair(SC_ADDR_LOCAL_TO_INT(next_pattern_element), next_pattern_element));

                out_common_result->insert(out_common_result->end(), next_common_result.begin(), next_common_result.end());
                new_common_result.insert(new_common_result.end(), next_common_result.begin(), next_common_result.end());
                cantorize_result_vector(&new_common_result);
                next_common_result.clear();
                next_common_result_arc.clear();
            }
        }//const loop

        common_result.insert(common_result.begin(), new_common_result.begin(), new_common_result.end());

        new_common_result.clear();
        remove_all_elements(&del_result, &common_result);
        remove_all_elements(&del_result, out_common_result);
        free_result_vector(&del_result);
    }//pattern loop

    out_common_result->insert(out_common_result->end(), common_result.begin(), common_result.end());
    common_result.clear();
    cantorize_result_vector(out_common_result);

    return SC_TRUE;
}

/*
 * returns SC_FALSE if element can't be found otherwise SC_TRUE
*/
sc_bool get_const_element(sc_addr pattern, sc_addr *result)
{
    sc_iterator3 *it = sc_iterator3_f_a_a_new(s_default_ctx,
                                              pattern,
                                              sc_type_arc_pos_const_perm,
                                              sc_type_const);
    if (SC_TRUE == sc_iterator3_next(it))
    {
        *result = sc_iterator3_value(it, 2);
        sc_iterator3_free(it);
        return SC_TRUE;
    }
    sc_iterator3_free(it);
    return SC_FALSE;
}

sc_result system_sys_search_only_full(sc_addr pattern, sc_type_result params, sc_type_result_vector *search_result)
{
    sc_addr start_pattern_node, start_const_node;

    if (SC_FALSE == get_const_element(pattern, &start_pattern_node))
    {
        if (params.empty())
        {
            std::cout << "ERROR: need const element to start search!" << std::endl;
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        else
        {
            sc_type_result::iterator it = params.begin();
            start_pattern_node = (*it).first;
            start_const_node = (*it).second;
        }
    }
    else
    {
        start_const_node = start_pattern_node;
    }
    sc_type_result *result = new sc_type_result();
    *result = params;

    sc_uint var_count = 0;
    sc_type_hash pattern_hash;

    copy_set_into_hash(pattern, sc_type_arc_pos_const_perm, 0, &pattern_hash, &var_count);
    pattern_hash.erase(SC_ADDR_LOCAL_TO_INT(start_pattern_node));
    system_sys_search_recurse(pattern, pattern_hash, start_const_node, start_pattern_node, result, search_result, 2);

    sort_result_vector(search_result);
    remove_result_vector_short_results(search_result, var_count);

    return SC_RESULT_OK;
}

sc_result system_sys_search_for_variables(sc_addr pattern, sc_type_result params, sc_addr_vector requested_values, sc_type_result_vector *search_result)
{
    sc_addr start_pattern_node, start_const_node;

    if (SC_FALSE == get_const_element(pattern, &start_pattern_node))
    {
        if (params.empty())
        {
            std::cout << "ERROR: need const element to start search!" << std::endl;
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        else
        {
            sc_type_result::iterator it = params.begin();
            start_pattern_node = (*it).first;
            start_const_node = (*it).second;
        }
    }
    else
    {
        start_const_node = start_pattern_node;
    }

    sc_type_result *result = new sc_type_result();
    *result = params;

    sc_uint var_count = 0;
    sc_type_hash pattern_hash;

    copy_set_into_hash(pattern, sc_type_arc_pos_const_perm, 0, &pattern_hash, &var_count);
    pattern_hash.erase(SC_ADDR_LOCAL_TO_INT(start_pattern_node));
    system_sys_search_recurse(pattern, pattern_hash, start_const_node, start_pattern_node, result, search_result, 2);

    sort_result_vector(search_result);
    remove_result_vector_short_results(search_result, var_count);
    if (!requested_values.empty())
    {
        filter_result_vector_by_variables(search_result, &requested_values);
    }

    return SC_RESULT_OK;
}

sc_result system_sys_search(sc_addr pattern, sc_type_result params, sc_type_result_vector *search_result)
{
    sc_addr start_pattern_node, start_const_node;
    if (SC_FALSE == get_const_element(pattern, &start_pattern_node))
    {
        if (params.empty())
        {
            std::cout << "ERROR: need const element to start search!" << std::endl;
            return SC_RESULT_ERROR_INVALID_PARAMS;
        }
        else
        {
            sc_type_result::iterator it = params.begin();
            start_pattern_node = (*it).first;
            start_const_node = (*it).second;
        }
    }
    else
    {
        start_const_node = start_pattern_node;
    }

    sc_type_result *result = new sc_type_result();
    *result = params;

    sc_uint var_count = 0;
    sc_type_hash pattern_hash;

    copy_set_into_hash(pattern, sc_type_arc_pos_const_perm, 0, &pattern_hash, &var_count);
    pattern_hash.erase(SC_ADDR_LOCAL_TO_INT(start_pattern_node));
    system_sys_search_recurse(pattern, pattern_hash, start_const_node, start_pattern_node, result, search_result, 2);

    sort_result_vector(search_result);
    remove_result_vector_short_results(search_result);

    return SC_RESULT_OK;
}

