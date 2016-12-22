/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_private_h_
#define _sc_memory_private_h_

#include "sc-store/sc_element.h"
#include "sc-store/sc_types.h"

struct _sc_memory_context
{
    sc_uint32 id;
    sc_access_levels access_levels;
};

extern sc_memory_context * s_memory_default_ctx;

/*! Function that create memory context with specified params
* @param levels Access levels, you can create it with macros @see sc_access_level_make
* @returns Retursn pointer to create memory context. If there were any errors during
* context creation, then function returns 0
* @note Do not use one context in different threads.
*/
sc_memory_context* sc_memory_context_new_impl(sc_uint8 levels);

/*! Function that destroys created memory context. You can use that function
* just for contexts, that were created with @see sc_memory_context_new
*/
void sc_memory_context_free_impl(sc_memory_context *ctx);

#endif
