/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_fm_engine_private_h_
#define _sc_fm_engine_private_h_

#include "sc_fm_engine.h"

//! Pointer to function, that create stream for data reading/witing
typedef sc_result (*fEngineStreamCreate)(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_uint8 flags, sc_stream **stream);
//! Pointer to function, that appends reference to the sc-addr that designates sc-link with specified data
typedef sc_result (*fEngineAddrRefAppend)(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum);
//! Pointer to function, that removes reference to the sc-addr for specified sc-link
typedef sc_result (*fEngineAddrRefRemove)(const sc_fm_engine *engine, sc_addr addr, const sc_check_sum *check_sum);
/*! Pointer to function, that finds all links with specified content.
 * If there are no any items found, then returns SC_RESULT_ERROR_NOT_FOUND
 */
typedef sc_result (*fEngineFind)(const sc_fm_engine *engine, const sc_check_sum *check_sum, sc_addr **result, sc_uint32 *result_count);
//! Function to clear file memory
typedef sc_result (*fEngineClear)(const sc_fm_engine *engine);
//! Funciton to save file memory state
typedef sc_result (*fEngineSave)(const sc_fm_engine *engine);
//! Pointer to function, that destroys storage specified data
typedef sc_result (*fEngineDestroyData)(const sc_fm_engine *engine);
//! Pointer to function, that clean file memory state on sc-memory start (invalid backward links to sc-addrs)
typedef sc_result (*fEngineCleanState)(const sc_fm_engine *engine);

/*! Sturcture that provides file memory storage engine object
 */
struct _sc_fm_engine
{
    //! Pointer to storage specified data
    void *storage_info;

    fEngineStreamCreate funcStreamCreate;
    fEngineAddrRefAppend funcAddrRefAppend;
    fEngineAddrRefRemove funcAddrRefRemove;
    fEngineFind funcFind;
    fEngineClear funcClear;
    fEngineSave funcSave;
    fEngineDestroyData funcDestroyData;
    fEngineCleanState funcCleanState;
};




#endif // _sc_fs_storage_private_h_
