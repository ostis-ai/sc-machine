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
#ifndef SC_SYSTEM_GEN_H
#define SC_SYSTEM_GEN_H

extern "C"
{
#include "sc_memory_headers.h"
}
#include "sc_system_operators.h"

/*! Generates fully isomorfic construction to given
 * @param pattern Generation pattern
 * @param params Generation parameters. Can be empty
 * @param result Set of results. Contains pairs between variables from pattern and generated constants.
 * @return If input params are correct and generation completed without errors, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result system_sys_gen(sc_memory_context *context, sc_addr pattern, sc_type_result params, sc_type_result *result);

/*! Generates fully isomorfic construction to given
 * @param pattern Generation pattern
 * @param params Generation parameters. Can be empty
 * @param result Set of results. Contains pairs between variables from pattern and generated constants onle for variables given in requested_values vector
 * @return If input params are correct and generation completed without errors, then return SC_OK;
 * otherwise return SC_ERROR
 */
sc_result system_sys_gen_for_variables(sc_memory_context *context, sc_addr pattern, sc_type_result params, sc_addr_vector requested_values, sc_type_result *result);

#endif // SC_SYSTEM_GEN_H
