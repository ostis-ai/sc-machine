/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_fs_storage_private_h_
#define _sc_dictionary_fs_storage_private_h_

#include "../sc_types.h"
#include "../sc-base/sc_allocator.h"

#include "stdio.h"

inline sc_uint32 sc_addr_to_hash(sc_addr addr)
{
  return SC_ADDR_LOCAL_TO_INT(addr);
}

sc_char * itora(sc_uint32 num)
{
  sc_uint32 copy_num = num;

  sc_uint32 size = 0;
  while (copy_num > 0)
  {
    copy_num /= 10;
    ++size;
  }

  sc_char * result = sc_mem_new(sc_char, size + 1);
  sc_char * index = result;

  while (num > 0)
  {
    *index++ = (sc_char)((num % 10) | '0');
    num /= 10;
  }

  return result;
}

inline sc_char * sc_addr_to_str(sc_addr addr)
{
  sc_uint32 hash = sc_addr_to_hash(addr);

  // !reverse translation for more effectively work
  return itora(hash);
}

sc_uint8 _sc_dictionary_addr_hashes_children_size()
{
  const sc_uint8 max_sc_char = 255;
  const sc_uint8 min_sc_char = 1;

  return max_sc_char - min_sc_char + 1;
}

void _sc_dictionary_addr_hashes_char_to_int(sc_char ch, sc_uint8 * ch_num, sc_uint8 * mask)
{
  *ch_num = 128 + (sc_uint8)ch;
}

sc_uint8 _sc_dictionary_strings_children_size()
{
  const sc_uint8 max_sc_char = 10;
  const sc_uint8 min_sc_char = 1;

  return max_sc_char - min_sc_char + 1;
}

void _sc_dictionary_strings_char_to_int(sc_char ch, sc_uint8 * ch_num, sc_uint8 * mask)
{
  *ch_num = (sc_uint8)ch - 48;
}

#endif
