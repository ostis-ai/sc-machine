/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_string_h_
#define _sc_string_h_

#include "../../sc-base/sc_allocator.h"
#include "../../sc_types.h"

#define sc_string_empty(string) ({ sc_string = sc_mem_new(sc_char, 1); })

#define sc_str_printf(out, size, format, ...) g_snprintf(out, size, format, __VA_ARGS__)

#define sc_str_cpy(copy, string, size) \
  ({ \
    copy = sc_mem_new(sc_char, size + 1); \
    sc_mem_cpy(copy, string, size); \
  })

#define sc_str_has_prefix(str, prefix) g_str_has_prefix(str, prefix)

#define sc_str_len(string) strlen(string)

#define sc_int_to_str_int(number, string) \
  sc_uint32 length = (number == 0) ? 1 : snprintf(null_ptr, 0, "%llu", number); \
  string = sc_mem_new(sc_char, length + 1); \
  gcvt(number, length, string)

#define sc_float_to_str_float(number, string) \
  sc_ulong digit_size = snprintf(NULL, 0, "%lu", (sc_ulong)number); \
  static sc_ulong mnemonic_size = 4; \
  string = sc_mem_new(sc_char, digit_size + mnemonic_size + 1); \
  gcvt(number, digit_size, string)

#define sc_str_find(str, substring) strstr(str, substring) != null_ptr

#define sc_str_find_position(str, substring) (sc_uint64)(strstr(str, substring) - str + 1)

#define sc_str_find_get(str, substring) strstr(str, substring)

#define sc_str_cmp(str, other) strcmp(str, other) == 0

#endif
