/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_string_h_
#define _sc_string_h_

#include "../../sc-base/sc_allocator.h"
#include "../../sc_types.h"

#define sc_str_cpy(copy, string, size) \
  ({ \
    copy = sc_mem_new(sc_char, size + 1); \
    sc_mem_cpy(copy, string, size); \
  })

#define sc_str_has_prefix(str, prefix) g_str_has_prefix(str, prefix)

#endif
