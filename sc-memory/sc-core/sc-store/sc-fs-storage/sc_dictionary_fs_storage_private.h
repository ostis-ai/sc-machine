/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_dictionary_fs_storage_private_h_
#define _sc_dictionary_fs_storage_private_h_

#include "../sc_types.h"
#include "../sc-base/sc_allocator.h"
#include "../sc-container/sc-string/sc_string.h"

#include "stdio.h"

sc_uint8 _sc_dictionary_addr_hashes_children_size()
{
  const sc_uint8 max_sc_char = 255;
  const sc_uint8 min_sc_char = 1;

  return max_sc_char - min_sc_char + 1;
}

void _sc_dictionary_addr_hashes_char_to_int(sc_char ch, sc_uint8 * ch_num, const sc_uint8 * mask)
{
  *ch_num = 128 + (sc_uint8)ch;
}

sc_uint8 _sc_dictionary_strings_children_size()
{
  const sc_uint8 max_sc_char = 10;
  const sc_uint8 min_sc_char = 1;

  return max_sc_char - min_sc_char + 1;
}

void _sc_dictionary_strings_char_to_int(sc_char ch, sc_uint8 * ch_num, const sc_uint8 * mask)
{
  *ch_num = (sc_uint8)ch - 48;
}

#endif
