/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_mutex_private_h_
#define _sc_mutex_private_h_

#include <glib.h>

struct _sc_mutex
{
  GMutex instance;
};

#include "sc-core/sc-base/sc_mutex.h"

#endif
