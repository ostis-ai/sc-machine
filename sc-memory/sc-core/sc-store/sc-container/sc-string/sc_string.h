/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_string_h_
#define _sc_string_h_

#include <stdio.h>

#include "../../sc-base/sc_allocator.h"
#include "../../sc_types.h"

#define sc_string_empty(string) string = sc_mem_new(sc_char, 1)

#define sc_str_printf(out, size, format, ...) g_snprintf(out, size, format, __VA_ARGS__)

#define sc_str_cpy(copy, string, size) \
  ({ \
    copy = sc_mem_new(sc_char, size + 1); \
    sc_mem_cpy(copy, string, size); \
  })

#define sc_str_concat(string1, string2, out_string) \
  sc_uint32 const size = sc_str_len(string1) + sc_str_len(string2) + 1; \
  out_string = sc_mem_new(sc_char, size + 1); \
  sc_str_printf(out_string, size, "%s%s", string1, string2)

#define sc_str_has_prefix(str, prefix) g_str_has_prefix(str, prefix)

#define sc_str_len(string) strlen(string)

#define sc_int_to_str_int(number, string, size) \
  snprintf(string, 20, "%" PRIu64, number); \
  size = sc_str_len(string)

#define sc_str_int_to_int(string, number) number = atoi(string)

#define sc_str_find(str, substring) strstr(str, substring) != null_ptr

#define sc_str_find_position(str, substring) (sc_uint64)(strstr(str, substring) - str + 1)

#define sc_str_find_get(str, substring) strstr(str, substring)

#define sc_str_cmp(str, other) strcmp(str, other) == 0

#endif
