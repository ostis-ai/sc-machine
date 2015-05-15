/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
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
