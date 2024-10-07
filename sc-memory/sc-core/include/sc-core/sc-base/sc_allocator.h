/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_allocator_h_
#define _sc_allocator_h_

#include "sc-core/sc_types.h"

_SC_EXTERN sc_pointer _sc_mem_new(sc_uint32 size);

#define sc_mem_new(struct_type, n_structs) (struct_type *)_sc_mem_new(sizeof(struct_type) * n_structs)

_SC_EXTERN sc_pointer sc_mem_set(sc_pointer pointer, sc_uint32 constant, sc_uint32 n_structs);

_SC_EXTERN sc_pointer sc_mem_cpy(sc_pointer source, sc_const_pointer dest, sc_uint32 n_structs);

_SC_EXTERN void sc_mem_free(sc_pointer pointer);

#endif
