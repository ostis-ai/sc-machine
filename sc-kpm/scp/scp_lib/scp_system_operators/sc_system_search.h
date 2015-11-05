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
#ifndef SC_SEARCH_SYSTEM_H
#define SC_SEARCH_SYSTEM_H

extern "C"
{
#include "sc_memory_headers.h"
}
#include "sc_system_operators.h"

/*! Search fully isomorfic construction, if impossible -
 *  the closest to fully isomorfic construction
 * @param pattern Search pattern
 * @param params Search parameters. Can be empty
 * @param result Set of results. Number depends on possible
 * isomorfic construction count. If fully isomorfic construction
 * can't be found the best results by element count are given.
 * @return If input params are correct and search completed without errors, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result system_sys_search(sc_memory_context *context, sc_addr pattern, sc_type_result params, sc_type_result_vector *result);

/*! Search fully isomorfic construction just to check such a construction existence
 * @param pattern Search pattern
 * @param params Search parameters. Can be empty
 * @param result Boolean result. SC_TRUE if fully isomorfic construction exists. SC_FALSE otherwise.
 * @return If input params are correct and search completed without errors, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result system_sys_search_single(sc_memory_context *context, sc_addr pattern, sc_type_result params, sc_bool *result);

/*! Search only fully isomorfic construction to given pattern
 * @param pattern Search pattern
 * @param params Search parameters. Can be empty
 * @param result Set of results. Number depends on possible isomorfic construction count.
 * @return If input params are correct and search completed without errors, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result system_sys_search_only_full(sc_memory_context *context, sc_addr pattern, sc_type_result params, sc_type_result_vector *result);

/*! Search only fully isomorfic construction to given pattern, leaving correspondences only for requested elements
 * @param pattern Search pattern
 * @param params Search parameters. Can be empty
 * @param requested_values Variables, for which correspondence pairs will be built
 * @param result Set of results. Number depends on possible isomorfic construction count.
 * Contains only correspondences for requested elements
 * @return If input params are correct and search completed without errors, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result system_sys_search_for_variables(sc_memory_context *context, sc_addr pattern, sc_type_result params, sc_addr_vector requested_values, sc_type_result_vector *result);


#endif // SC_SEARCH_SYSTEM_H
