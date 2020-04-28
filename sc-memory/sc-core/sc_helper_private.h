/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_helper_private_h_
#define _sc_helper_private_h_

#include "sc-store/sc_types.h"

/*! Initialize helper.
 * @remarks Need to be called once at the beginning of sc-helper usage
 * @return If sc-helper initialized without any errors, then return SC_OK;
 * otherwise returns SC_ERROR
 */
sc_result sc_helper_init(sc_memory_context const * ctx);

/*! Shuts down sc-helper.
 * @remarks This function need to be called once at the end of sc-helper usage
 */
void sc_helper_shutdown();


#endif
