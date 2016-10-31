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
    sc_uint16 id;
    sc_access_levels access_levels;
};

extern sc_memory_context * s_memory_default_ctx;

#endif
