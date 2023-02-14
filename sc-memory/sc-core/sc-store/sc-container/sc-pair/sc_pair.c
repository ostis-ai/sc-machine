/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_pair.h"

#include "../../sc-base/sc_allocator.h"

sc_bool sc_pair_initialize(sc_pair ** pair, void * first, void * second)
{
  *pair = sc_mem_new(sc_pair, 1);
  (*pair)->first = first;
  (*pair)->second = second;

  return SC_TRUE;
}

sc_pair * sc_make_pair(void * first, void * second)
{
  sc_pair * pair;
  sc_pair_initialize(&pair, first, second);

  return pair;
}
