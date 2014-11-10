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

#ifndef _sc_fm_engine_h_
#define _sc_fm_engine_h_

#include "sc_types.h"
#include "sc_stream.h"

typedef struct _sc_fm_engine sc_fm_engine;

//! Destoroys file memory object
void sc_fm_free(sc_fm_engine *engine);

/*! Create data stream to write/read data of specified sc-link
 * @param engine Pointer to used file memory engine
 * @param check_sum Pointer to data checksum
 * @param flags Stream read/write flag
 * @param stream Pointer that will contains created stream
 * @returns If stream created, then resurns SC_RESULT_OK; otherwise returns error code
 * @attention Returned pointer to stream need to be free after usage
 */
sc_result sc_fm_stream_new(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_uint8 flags, sc_stream **stream);

/*! Appends new reference from file memory to sc-link addr
 * @param engine Pointer to used file memory engine
 * @param addr sc-addr of sc-link, that will be added to reference
 * @param check_sum Pointer to checksum of data
 * @returns If reference added, then returns SC_RESULT_OK; otherwise returns error code
 */
sc_result sc_fm_addr_ref_append(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum);

/*! Removes reference from file memory to sc-link addr
 * @param engine Pointer to used file memory engine
 * @param addr sc-addr of sc-link, that will be removed from references
 * @param check_sum Pointer to checksum of data
 * @returns If reference removed, then returns SC_RESULT_OK; otherwise returns error code
 */
sc_result sc_fm_addr_ref_remove(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum);

/*! Find all sc-link addrs by specified content data
 * @param engine Pointer to used file memory engine
 * @param check_sum Check sum of data
 * @param result Pointer to result container
 * @param result_count Container for results count
 * @returns If sc-links with specified checksum found, then sc-addrs of found link
 * writes into \p result array and function returns SC_OK; otherwise \p result will contain
 * empty sc-addr and function returns SC_OK. In any case \p result_count contains number of found
 * sc-addrs
 * @attention \p result array need to be free after usage
 */
sc_result sc_fm_find(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count);

/*! Clear file memory
 * @param engine Pointer to used file memory engine
 * @returns If there are any errors during file memory clear, then returns SC_RESULT_ERROR; othrwise returns SC_RESULT_OK
 */
sc_result sc_fm_clear(const sc_fm_engine *engine);

/*! Save state of file memory
 * @param engine Pointer to used file memory engine
 * @returns If there are any errors during file memory state save, then returns SC_RESULT_ERROR; othrwise returns SC_RESULT_OK
 */
sc_result sc_fm_save(const sc_fm_engine *engine);

/*! Clean state of file memory
 * @param engine Pointer to used file memory engine
 * @returns If there are any errors during file memory state clean, then returns SC_RESULT_ERROR; othrwise returns SC_RESULT_OK
 */
sc_result sc_fm_clean_state(const sc_fm_engine *engine);


#endif // _sc_fm_engine_h_
