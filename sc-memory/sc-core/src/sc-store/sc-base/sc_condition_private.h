/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_condition_private_h_
#define _sc_condition_private_h_

#include <glib.h>

typedef struct _sc_condition
{
  GCond instance;
} sc_condition;

#include "sc-core/sc-base/sc_condition.h"

#endif
