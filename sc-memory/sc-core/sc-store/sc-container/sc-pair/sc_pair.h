/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_pair_h_
#define _sc_pair_h_

#include "../../sc_types.h"

typedef struct _sc_pair
{
  void * first;
  void * second;
} sc_pair;

sc_bool sc_pair_initialize(sc_pair ** pair, void * first, void * second);

sc_pair * sc_make_pair(void * first, void * second);

#endif
