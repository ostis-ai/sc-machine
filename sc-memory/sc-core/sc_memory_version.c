/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory_version.h"

#include "sc-store/sc-base/sc_allocator.h"
#include "sc-store/sc-base/sc_assert_utils.h"

sc_int32 sc_version_compare(const sc_version * a, const sc_version * b)
{
  sc_assert(a && b);

  if (a->major < b->major)
    return -1;
  if (a->major > b->major)
    return 1;

  if (a->minor < b->minor)
    return -1;
  if (a->minor > b->minor)
    return 1;

  if (a->patch < b->patch)
    return -1;
  if (a->patch > b->patch)
    return 1;

  return 0;
}

char * sc_version_string_new(const sc_version * v)
{
  if (v->suffix != null_ptr)
    return g_strdup_printf("%u.%u.%u %s", v->major, v->minor, v->patch, v->suffix);

  return g_strdup_printf("%u.%u.%u", v->major, v->minor, v->patch);
}

void sc_version_string_free(sc_char * str)
{
  sc_assert(str != null_ptr);
  sc_mem_free(str);
}

sc_uint32 sc_version_to_int(sc_version const * version)
{
  sc_assert(version != null_ptr);
  return (version->major << 16) | (version->minor << 8) | version->patch;
}

void sc_version_from_int(sc_uint32 value, sc_version * version)
{
  sc_assert(version != null_ptr);
  version->patch = value & 0xff;
  version->minor = value >> 8 & 0xff;
  version->major = value >> 16 & 0xff;
}
