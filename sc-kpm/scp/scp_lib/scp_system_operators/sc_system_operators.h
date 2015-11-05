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
#ifndef SC_SYSTEM_OPERATORS_H
#define SC_SYSTEM_OPERATORS_H

extern "C"
{
#include "sc_memory_headers.h"
}
#include <map>
#include <vector>

struct sc_addr_comparator
{
    bool operator()(const sc_addr s1, const sc_addr s2) const
    {
        return (SC_ADDR_LOCAL_TO_INT(s1) < SC_ADDR_LOCAL_TO_INT(s2));
    }
};

typedef std::map<sc_addr, sc_addr, sc_addr_comparator> sc_type_result;
typedef std::map<int, sc_addr> sc_type_hash;
typedef std::vector<sc_type_result *> sc_type_result_vector;
typedef std::vector<sc_addr> sc_addr_vector;
typedef std::pair<sc_addr, sc_addr> sc_addr_pair;
typedef std::pair<int, sc_addr> sc_hash_pair;

void print_hash(sc_type_hash table);
void print_result(sc_memory_context *context, sc_type_result table);
void print_result_set(sc_memory_context *context, sc_type_result_vector *table);
sc_bool copy_set_into_hash(sc_memory_context *context, sc_addr set, sc_type arc_type, sc_type end_type, sc_type_hash *table, sc_uint *var_count);
sc_bool find_result_pair_for_var(sc_type_result *set, sc_addr var_element, sc_addr *result);
sc_bool find_result_pair_for_const(sc_type_result *set, sc_addr const_element, sc_addr *result);
sc_bool check_coherence(sc_memory_context *context, sc_addr element, sc_addr sc_pattern, sc_addr checked_arc, sc_bool output, sc_type_result *result, sc_type_result *inp_result);
void cantorize_result_vector(sc_type_result_vector *result);
void remove_all_elements(sc_type_result_vector *pattern, sc_type_result_vector *source);
void free_single_result(sc_type_result *result);
void free_result_vector(sc_type_result_vector *result);
void sort_result_vector(sc_type_result_vector *data);
sc_bool remove_result_vector_short_results(sc_type_result_vector *data);
sc_bool remove_result_vector_short_results(sc_type_result_vector *data, sc_uint size);
sc_bool sc_addr_vector_contains(sc_addr addr, sc_addr_vector *requested_values);
void filter_result_vector_by_variables(sc_type_result_vector *data, sc_addr_vector *requested_values);
void filter_result_by_variables(sc_type_result *data, sc_addr_vector *requested_values);

void printIdtf(sc_memory_context *context, sc_addr element);
#endif // SC_SYSTEM_OPERATORS_H
