/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-core/sc-base/sc_allocator.h"

#include <glib.h>
#include <memory.h>

sc_pointer _sc_mem_new(sc_uint32 size)
{
  sc_pointer pointer = g_malloc(size);
  return pointer ? sc_mem_set(pointer, 0, size) : null_ptr;
}

sc_pointer sc_mem_set(sc_pointer pointer, sc_uint32 constant, sc_uint32 n_structs)
{
  return memset(pointer, constant, n_structs);
}

sc_pointer sc_mem_cpy(sc_pointer source, sc_const_pointer dest, sc_uint32 n_structs)
{
  return memcpy(source, dest, n_structs);
}

void sc_mem_free(sc_pointer pointer)
{
  g_free(pointer);
}
